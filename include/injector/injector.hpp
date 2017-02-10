/** \file
*/
// TODO should write/read vp be true by default?
#pragma once

#include <injector/detail/macros.hpp>
#include <injector/unprotect.hpp>

#include <injector/pointer/pointer.hpp>
#include <injector/pointer/raw_ptr.hpp>
#include <injector/pointer/integral.hpp>

// NOTES ON DEVELOPMENT
// uintptr_t represents a pointer that does not mean it is accessible, it could be in another process and everything.
// raw_ptr, however, is acessible on the current process.
// that uintptr_t should be loaded into the correct ptr type, raw or proc..
// that's why make_jmp, for instance, returns uintptr_t, we don't know if that value is acessible on the current proc.

// TODO faster_ptr is actually rawer_ptr etc

namespace injector
{
    /// Writes `value` to the memory location `addr`.
    ///
    /// When `vp` is set to `true`, the memory region will be temporarly unprotected for writing.
    ///
    /// @tparam T               Must implement the [PODType][1] concept.
    /// @tparam IntoPointer     Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// **Examples**:
    /// \code
    /// using injector::raw_ptr;
    /// injector::write<int>(0x405A76, 1990);           // Integral address
    /// injector::write<int>((int*)(0x405A76), 1990);   // Pointer address
    /// injector::write<int>(raw_ptr(0x405A76), 1990);  // MemoryPointer address
    /// \endcode
    ///
    /// [1]: http://en.cppreference.com/w/cpp/concept/PODType
    template<typename T, typename IntoPointer>
    inline void write(IntoPointer addr, const T& value, bool vp = true)
    {
        _INJECTOR_ENSURE_POD_TYPE(T);
        auto ptr = into_ptr(addr);
        auto guard = unprotect(ptr, vp? sizeof(T) : 0);
        return ptr.write(value);
    }

    /// Reads a value typed `T` from the memory location `addr`.
    ///
    /// When `vp` is set to `true`, the memory region will be temporarly unprotected for reading.
    ///
    /// @tparam T               Must implement the [`PODType`][1] concept.
    /// @tparam IntoPointer     Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// **Examples**:
    /// \code
    /// using injector::raw_ptr;
    /// int a = injector::read<int>(0x405A76);
    /// int b = injector::read<int>((int*)(0x405A76));
    /// int c = injector::read<int>(raw_ptr(0x405A76));
    /// \endcode
    ///
    /// [1]: http://en.cppreference.com/w/cpp/concept/PODType
    template<typename T, typename IntoPointer>
    inline T read(IntoPointer addr, bool vp = true)
    {
        _INJECTOR_ENSURE_POD_TYPE(T);
        auto ptr = into_ptr(addr);
        auto guard = unprotect(ptr, vp? sizeof(T) : 0);
        return ptr.read<T>();
    }

    /// Fills the memory location at `addr` with the byte `value` along `size` bytes.
    ///
    /// When `vp` is set to `true`, the memory region will be temporarly unprotected for writing.
    ///
    /// @tparam IntoPointer     Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// **Examples**:
    /// \code
    /// // Sets 0x405A76 until 0x405A7E (exclusive) with 0x90 (x86 NOP) bytes.
    /// injector::fill(0x405A76, 0x90, 8);
    /// \endcode
    ///
    /// @see make_nop
    template<typename IntoPointer>
    inline void fill(IntoPointer addr, std::uint8_t value, std::size_t size, bool vp = true)
    {
        auto ptr = into_ptr(addr);
        auto guard = unprotect(ptr, vp? size : 0);
        return ptr.fill(value, size);
    }


    /// Adjusts a object pointer to a new object pointer.
    ///
    /// Searches in the memory region [`addr`, `addr` + `max_search`] for a pointer in the range [`old_begin`, `old_end`]
    /// and replaces it with `(new_begin + (found_pointer - old_begin))`, that is, `new_begin` with the same offset as
    /// it was there with `old_begin`.
    ///
    /// The addresses `addr`, `old_begin`, `old_end` and `new_begin` pointers must be part of the same process/address space.
    ///
    /// The default value for `max_search` is *almost* the maximum size of a single instruction.
    ///
    /// When `vp` is set to `true`, the memory region will be temporarly unprotected for reading/writing.
    ///
    /// Returns `(new_begin + (found_pointer - old_begin))` if anything found, `nullptr` otherwise.
    ///
    /// @tparam IntoPointerA, IntoPointerB, IntoPointerE Must implement the `IntoPointer` (`conv_pointer`) concept.
    /// #tparam MemoryPointerN                           Must implement the `MemoryPointer` concept.
    ///
    /// @warning This function works a little bit differently from the equivalent in the old injector version (`injector::AdjustPointer`),
    /// be aware of the following when porting your code off the old version:
    /// @warning The parameter order is different from the old version.
    /// @warning The `old_end` is not inclusive (unlike the old version) meaning the address it represents won't be considered part of the object in the search.
    /// Consider adding a `+1` into this parameter when porting from the old version.
    ///
    /// **Examples**:
    /// \code
    /// // ### Imagine the following ###
    /// //
    /// // We have the following struct type:
    /// //
    /// // struct CLight // sizeof = 0x14
    /// // {
    /// //    float field_0[3];
    /// //    int field_C;
    /// //    int field_10;
    /// // }
    /// //
    /// // And an array of objects of this type is present at 0x700000, like this:
    /// // .data:00700000     ; CLight aLights[8]
    /// // Meaning it extends itself until 0x7000A0 (i.e. 0x700000 + sizeof(aLights)).
    /// //
    /// // Now, we want to replace this array with a bigger one, thus we need to adjust every instruction that access this array.
    /// //
    /// // For this example, we'll assume a instruction at 0x500000 like this:
    /// // .text:00500000     mov edx, aLights.field_C[eax]
    /// //
    /// // The address referenced by the instruction is not 0x700000 but 0x70000C (field_C), and this happens often, easy to miss,
    /// // making sure you're doing it correctly may be annoying. This function, however, will take care of this difference for you.
    ///
    /// #include <injector/injector.hpp>
    ///
    /// CLight* aNewLights = new CLight[n];
    /// injector::adjust(0x500000, 0x700000, 0x7000A0, raw_ptr(aNewLights));
    /// 
    /// // Now the instruction at 0x500000 is looking like this:
    /// // .text:00500000     mov edx, aNewLights.field_C[eax]
    /// \endcode
    template<typename IntoPointerA, typename IntoPointerB, typename IntoPointerE, typename MemoryPointerN>
    inline auto adjust(IntoPointerA addr,
                       IntoPointerB old_begin, IntoPointerE old_end, MemoryPointerN new_begin,
                       std::size_t max_search = (_INJECTOR_OP_MAXSIZE - 3), bool vp = true)
                       -> typename MemoryPointerN::fast_ptr
    {
        // AAA this function changed parameter order and the meaning of default_end, see @warnings

        _INJECTOR_ENSURE_MEM_PTR(MemoryPointerN);

        auto ptr = into_ptr(addr);
        auto ptr_end   = ptr + max_search;
        auto begin = into_ptr(old_begin);
        auto end = into_ptr(old_end);
        auto newbeg = faster_ptr(new_begin);

        static_assert(f_are_same_raw(ptr, ptr_end, begin, end, newbeg), "");

        for(; ptr < ptr_end; ++ptr)
        {
            auto value = read<uintptr_t>(ptr, vp);
            if(value >= uintptr_t(begin) && value < uintptr_t(end))
            {
                auto replace = newbeg + (value - uintptr_t(begin));
                write<uintptr_t>(ptr, uintptr_t(replace), vp);
                return replace;
            }
        }

        return nullptr;
    }

}

/*

REMEMBER GVM?

BASE

    void write_raw(uintptr_t addr, const void* src, size_t size, bool vp);
    void read_raw(uintptr_t addr, void* dst, size_t size, bool vp);

    template<typename T>
    void write(uintptr_t addr, T value, bool vp = true);

    template<typename T>
    T read(uintptr_t addr, bool vp = true);

    void fill(uintptr_t addr, uint8_t value, size_t size, bool vp);

    // CHANGED PARAMS ORDER; PROBS WILL CHANGE MEANING OF PARAMS BEGIN,END
    void adjust(uintptr_t addr, uintptr_t default_begin, uintptr_t default_end, uintptr_t new_begin, size_t max_search = 8, bool vp = true);

    // x86 specific
    uintptr_t make_jmp(uintptr_t addr, uintptr_t dest, bool vp = true)

    // x86 specific
    uintptr_t make_call(uintptr_t addr, uintptr_t dest, bool vp = true);

    // x86 specific
    uintptr_t make_nop(uintptr_t addr, size_t size, bool vp = true);

    // x86 specific
    uintptr_t make_ret(uintptr_t addr, uint16_t pop, bool vp = true);

    //
    // OTHER FUNCS:
    // GetAbsoluteOffset
    // GetRelativeOffset
    // ReadRelativeOffset
    // MakeRelativeOffset
    // GetBranchDestination
    // MakeJA
    // MakeRangedNOP
    // Imagined: MakeShortJmp, MakeShortCall, etc
    //
    // OTHER CLASSES:
    // auto_pointer??
    // memory_pointer
    // memory_pointer_raw
    // memory_pointer_aslr
    // lazy_pointer
    // lazy_object
    //

CALLING
    same as today

UTILITY
    same as today

HOOKING




*/


#if 0
template<typename T, std::size_t Align = 1>
auto view_pod(const T& object) -> std::aligned_storage_t<sizeof(T), Align>&
{
    return reinterpret_cast<std::aligned_storage_t<sizeof(T), Align>&>(bytes);
}

template<typename T, std::size_t Align>
auto view_object(const std::aligned_storage_t<sizeof(T), Align>& bytes) -> T&
{
    return reinterpret_cast<T&>(bytes);
}

template<typename T, std::size_t Align = 1>
auto into_pod(const T& object) -> std::aligned_storage_t<sizeof(T), Align>
{
    /*std::aligned_storage_t<sizeof(T), Align> bytes;
    std::memcpy(&bytes, &object, size);
    return bytes;*/
    return view_pod(object);
}

template<typename T, std::size_t Align>
T from_pod(const std::aligned_storage_t<sizeof(T), Align>& bytes)
{
    //return *reinterpret_cast<T*>(bytes);
    return view_object(bytes);
}
#endif

