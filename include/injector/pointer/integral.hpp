#pragma once
#include <injector/pointer/pointer.hpp>
#include <injector/pointer/raw_ptr.hpp>

namespace injector
{
    namespace detail
    {
        /// Whether `T` is elegible for the fundamentals `IntoPointer` down there.
        template<typename T>
        struct is_integral_pointer
            : std::bool_constant<(std::is_integral<T>::value && !std::is_same<T, bool>::value && !std::is_same<T, char>::value) ||
            (std::is_pointer<T>::value || std::is_null_pointer<T>::value)>
        {
        };
    }

    /// `IntoPointer` implementation for fundamental C++ types and pointers, such as
    /// `int`, `long`, `unsigned long`, `void*`, `T*`, `std::uint32_t`, `std::uintptr_t`, `std::nullptr_t` and many others.
    ///
    /// @see conv_pointer
    /// @see `into_ptr`
    template<typename T>
    struct conv_pointer<T, std::enable_if_t<detail::is_integral_pointer<T>::value>>
    {
        /// The type returned by `into_ptr()` using the type `T`.
        using conv_type = raw_ptr;

        /// Use `into_ptr()` instead.
        static conv_type convert(const T& x)
        {
            return raw_ptr(x);
        }
    };
}
