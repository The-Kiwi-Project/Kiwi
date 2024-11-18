#pragma once

#include <memory>

namespace std {

    template <typename T>
    using Rc = shared_ptr<T>;

    template <typename T>
    using Box = unique_ptr<T>;

}