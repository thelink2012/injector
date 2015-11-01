#pragma once
#include <injector/pointer/raw_ptr.hpp>

namespace injector
{
    /// Guards a region of memory that was unprotected temporarly.
    ///
    /// When this object gets created a region of memory gets unprotected,
    /// when destroyed this same region will get back to it's previous protection access,
    /// unless `forget()` was called.
    ///
    /// **Examples**:
    /// \code
    /// #include <injector/injector.hpp>
    /// using injector::raw_ptr;
    /// using injector::scoped_unprotect;
    /// scoped_unprotect<raw_ptr> guard1(0x100000, 16);
    /// auto guard2 = injector::unprotect(0x200000, 16);    // another way to make a unprotect guard
    /// \endcode
    ///
    /// @see injector::unprotect
    template<typename MemoryPointer = raw_ptr>
    class scoped_unprotect
    {
        public:
            /// Unprotects the specified memory `addr`ess along the specified `size` and stores the old protection.
            ///
            /// If `size = 0` or unprotection failed, no operation will be performed and `this->unprotected()`
            /// will evaluate `false`.
            scoped_unprotect(MemoryPointer addr, std::size_t size)
            {
                this->addr = faster_ptr(addr);
                if(size != 0 && this->addr.unprotect(size, &this->oldprot))
                    this->size = size;
                else
                    this->size = 0;         // either size is zero or unprotection didn't happen.
            }

            /// Restores the guarded protection.
            ~scoped_unprotect()
            {
                this->restore();
            }

            /// Copy is not allowed!
            scoped_unprotect(const scoped_unprotect&) = delete;

            /// Copy is not allowed!
            scoped_unprotect& operator=(const scoped_unprotect&) = delete;

            /// Move constructor.
            scoped_unprotect(scoped_unprotect&& rhs)
            {
                this->size = 0;
                *this = std::move(rhs);
            }

            /// Move assignment operator.
            scoped_unprotect& operator=(scoped_unprotect&& rhs)
            {
                this->restore();
                this->addr = rhs.addr;
                this->size = rhs.size;
                this->oldprot = rhs.oldprot;
                rhs.size = 0;
            }

            /// Checks whether this guard is currently guarding anything.
            bool unprotected() const
            {
                return this->size != 0;
            }

            /// Restores the protection of the unprotected region (just like when this object gets destructed).
            ///
            /// The next calls to `this->unprotected()` will evaluate to `false`.
            void restore()
            {
                if(this->unprotected())
                {
                    this->addr.reprotect(this->size, this->oldprot);
                    this->size = 0;
                }
            }

            /// Forgets about the guarded unprotection in such a way that the memory region will not be
            /// reprotected when this object gets destructed.
            ///
            /// The next calls to `this->unprotected()` will evaluate to `false`.
            void forget()
            {
                this->size = 0;
            }

        private:
            using faster_ptr = typename MemoryPointer::fast_ptr;
            faster_ptr      addr;
            std::size_t     size;       // if zero, addr and oldprot should be ignored
            uint32_t        oldprot;
    };


    /// Unprotects the memory region at `addr`ess for the determined `size`.
    ///
    /// The old, platform specific, protection access will be copied to `oldprot`.
    ///
    /// Returns whether the unprotection was successful.
    ///
    /// @tparam IntoPointer     Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// **Examples**:
    /// \code
    /// #include <injector/injector.hpp>
    /// uint32_t oldprot;
    /// if(injector::unprotect(0x300000, 16, oldprot)) // nothing happens on failure.
    /// {
    ///     // 0x300000+ is now unprotected.
    ///
    ///     injector::reprotect(0x300000, 16, oldprot);
    ///
    ///     // 0x300000+ is now reprotected with the same access as before.
    /// }       
    /// \endcode
    ///
    /// @see injector::reprotect
    template<typename IntoPointer>
    inline bool unprotect(IntoPointer addr, size_t size, uint32_t& oldprot)
    {
        into_ptr(addr).unprotect(size, &oldprot);
    }

    /// Reprotects the memory `addr` with `size` previosly unprotected by `injector::unprotect(addr, size, oldprot)`.
    ///
    /// The `oldprot` parameter is a platform specific value previosly returned by the unprotection call.
    ///
    /// Returns whether the reprotection was successful.
    ///
    /// @tparam IntoPointer     Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// @see injector::unprotect
    template<typename IntoPointer>
    inline bool reprotect(IntoPointer addr, size_t size, uint32_t oldprot)
    {
        into_ptr(addr).reprotect(size, oldprot);
    }

    /// Unprotects the memory region at `addr`ess for the determined `size`.
    ///
    /// This version **returns a guard** to the unprotected memory in such a way that when the guard
    /// gets destructed, the memory should get reprotected with whatever protection it had before.
    ///
    /// @tparam IntoPointer     Must implement the `IntoPointer` (`conv_pointer`) concept.
    ///
    /// **Examples**:
    /// \code
    /// #include <injector/injector.hpp>
    /// auto guard = injector::unprotect(0x100000, 16);     // 0x100000+ will be unprotected until guard gets out of scope.
    /// injector::guard_unprotect(0x200000, 16).forget();   // 0x200000+ will be unprotected forever.
    /// injector::guard_unprotect(0x300000, 16);            // WRONG!!! 0x300000+ will still be protected.
    /// \endcode
    ///
    /// @see injector::scoped_unprotect
    /// @see injector::unprotect
    template<typename IntoPointer>
    inline auto unprotect(IntoPointer addr, size_t size) -> scoped_unprotect<typename conv_pointer<IntoPointer>::conv_type>
    {
        auto xaddr = into_ptr(addr);
        return scoped_unprotect<decltype(xaddr)>(xaddr, size);
    }
}
