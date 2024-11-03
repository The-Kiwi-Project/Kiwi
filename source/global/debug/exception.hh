#pragma once

#include <std/string.hh>

namespace kiwi {

    struct Exception {
        virtual auto what() const -> std::String = 0;
        virtual ~Exception() noexcept {};
    };

}