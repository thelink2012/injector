#pragma once

// TODO move those fundamental includes to another include?
#include <cstddef>
#include <cstdint>
#include <utility>

static_assert(sizeof(uintptr_t) == sizeof(void*), "Ops... didn't expect that");

#define _INJECTOR_ENSURE_POD_TYPE(T) \
        static_assert(std::is_pod<T>::value, "Type " #T " must be POD for safety reasons.")
        // TODO instruct how to turn objects into pod and vice-versa

#define _INJECTOR_ENSURE_INTO_PTR(T) // TODO ensure IntoPointer concept

#define _INJECTOR_ENSURE_MEM_PTR(T) // TODO ensures the MemoryPointer concept, does not accept IntoPointer

// TODO on x64 is this bigger? don't think so but still
#define _INJECTOR_OP_MAXSIZE (15)



