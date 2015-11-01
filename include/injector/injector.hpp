// TODO should write/read vp be true by default?
#pragma once

#include <injector/detail/macros.hpp>
#include <injector/unprotect.hpp>

#include <injector/pointer/pointer.hpp>
#include <injector/pointer/raw_ptr.hpp>
#include <injector/pointer/integral.hpp>

namespace injector
{
    /// Writes `value` to the specified memory `addr`ess.
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
        auto xaddr = into_ptr(addr);
        auto xprotect = unprotect(xaddr, vp? sizeof(T) : 0);
        return xaddr.write(value);
    }

    /// Reads a value typed `T` from the specified memory `addr`ess.
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
        auto xaddr = into_ptr(addr);
        auto xprotect = unprotect(xaddr, vp? sizeof(T) : 0);
        return xaddr.read<T>();
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

