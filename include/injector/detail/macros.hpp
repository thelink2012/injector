#pragma once

// TODO move those fundamental includes to another include?
#include <cstddef>
#include <cstdint>
#include <utility>

#define _INJECTOR_ENSURE_POD_TYPE(T) \
        static_assert(std::is_pod<T>::value, "Type " #T " must be POD for safety reasons.")
        // TODO instruct how to turn objects into pod and vice-versa




