#pragma once

namespace injector
{
    /// **Concept:**
    /// \code
    /// template<>
    /// struct conv_pointer<Input>
    /// {
    ///     using conv_type = Output;
    ///
    ///     static conv_type convert(const Input& x)
    ///     {
    ///         /* ... */
    ///     }
    /// }
    /// \endcode
    ///
    /// This is the `conv_pointer` struct, used to implement the `IntoPointer` **OR** `IntoFasterPointer` concept.
    ///
    /// The `convert` static method must convert `Input` into `Output`.
    ///
    /// - For `IntoPointer`:
    ///     + `Input` may be anything one wants to be able to convert into `MemoryPointer`.
    ///     + `Output` must be a `MemoryPointer`.
    ///
    /// - For `IntoFasterPointer`:
    ///     + `Input` must be a `MemoryPointer`.
    ///     + `Output` must be either `Input` itself or a more efficient `MemoryPointer`.
    ///
    /// After this concept is implemented, the `injector::faster_ptr()` and/or `injector::into_ptr()` functions can be used.
    /// Both functions does the same thing, calls `conv_pointer::convert`, however for readability both functions are
    /// provided to manifest the programmer's intent.
    ///
    template<typename Input, typename Enable = void>
    struct conv_pointer;

    /// Converts the `MemoryPointer` `ptr` into another more efficient `MemoryPointer` type.
    ///
    /// In case `ptr` is the maximum archiveable efficiency, the same value and type are returned.
    ///
    /// @tparam MemoryPointer   Must implement the `MemoryPointer` (and `IntoFasterPointer` (`conv_pointer`)) concept.
    ///
    /// @see conv_pointer
    /// @see into_ptr
    template<typename MemoryPointer>
    auto faster_ptr(const MemoryPointer& ptr)
    {
        return conv_pointer<T>::convert(ptr);
    }

    /// Converts the value `x` into some `MemoryPointer` type, usually a`injector::raw_ptr`.
    ///
    /// @tparam T   Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// @see conv_pointer
    /// @see faster_ptr
    template<typename T>
    auto into_ptr(const T& x)
    {
        return conv_pointer<T>::convert(x);
    }
}
