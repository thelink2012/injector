#pragma once
#include <type_traits>

// AAA explicit operator uintptr_t() does not perform translation in original

// concept:
// @ injector::is_pointer<T>
// @ way to do [+ - / *] operations on itself and on uintptrs (!! += instead and + implemented outside with is_pointer etc)
// @ way to do [< <= == != >= >] on itself. (!! something like above?)
// @ explicit operator bool
// @ explicit operator uintptr is not required
//
// @ read / write (maybe no actual need for T&& on things like process pointers; no way to move)
//

#include <windows.h>

#include <injector/pointer/pointer.hpp>

#include <injector/detail/macros.hpp>




namespace injector
{
    class raw_ptr
    {
        public:
             raw_ptr() = default;
             
             raw_ptr(std::nullptr_t) {}
             
             raw_ptr(const raw_ptr& rhs) = default;

             raw_ptr(uintptr_t a) : a(a) {};
             
             template<typename T>
             raw_ptr(T* p) : p(reinterpret_cast<void*>(p)) {};

        public: // Operations

            using fast_ptr = raw_ptr;

            fast_ptr resolve() const
            {
                return *this;
            }

            template<typename T>
            T read() const
            {
                _INJECTOR_ENSURE_POD_TYPE(T);
                return *reinterpret_cast<T*>(this->p);
            }

            template<typename T>
            void write(const T& value) const
            {
                _INJECTOR_ENSURE_POD_TYPE(T);
                *reinterpret_cast<T*>(this->p) = value;
            }

        public:
            template<typename MemoryPointer>
            friend class scoped_unprotect;

            bool unprotect(size_t size, uint32_t* old_prot) const // TODO maybe int* for a typedef
            {
                static_assert(sizeof(*old_prot) == sizeof(DWORD), "");
                return !!VirtualProtect(this->p, size, PAGE_EXECUTE_READWRITE, reinterpret_cast<DWORD*>(old_prot));
            }

            bool reprotect(size_t size, uint32_t prot) const
            {
                DWORD dw_dummy;
                return !!VirtualProtect(this->p, size, prot, &dw_dummy);
            }

        public: // Operators

            explicit operator bool() const
            {
                return !!this->p;
            }

            // (not compulsory / not part of concept)
            explicit operator uintptr_t() const
            {
                return this->a;
            }

            raw_ptr operator+(const raw_ptr& rhs) const
            {
                return raw_ptr(this->a + rhs.a);
            }

            raw_ptr operator-(const raw_ptr& rhs) const
            {
                return raw_ptr(this->a - rhs.a);
            }

            raw_ptr operator*(const raw_ptr& rhs) const
            {
                return raw_ptr(this->a * rhs.a);
            }

            raw_ptr operator/(const raw_ptr& rhs) const
            {
                return raw_ptr(this->a / rhs.a);
            }

            bool operator==(const raw_ptr& rhs) const
            {
                return this->a == rhs.a;
            }

            bool operator!=(const raw_ptr& rhs) const
            {
                return this->a != rhs.a;
            }

            bool operator<(const raw_ptr& rhs) const
            {
                return this->a < rhs.a;
            }

            bool operator<=(const raw_ptr& rhs) const
            {
                return this->a <= rhs.a;
            }

            bool operator>(const raw_ptr& rhs) const
            {
                return this->a > rhs.a;
            }

            bool operator>=(const raw_ptr& rhs) const
            {
                return this->a >= rhs.a;
            }

        protected:
            union {
                void*     p = nullptr;
                uintptr_t a;
            };
    };

    template<>
    struct conv_pointer<raw_ptr>
    {
        using conv_type = raw_ptr;
        static conv_type convert(const raw_ptr& x)
        {
            return x;
        }
    };
}

