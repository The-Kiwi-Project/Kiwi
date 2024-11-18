#pragma once 

#include <std/integer.hh>

namespace kiwi {

    auto random() -> double;
    // [min, max)
    auto random_i64(std::i64 min, std::i64 max) -> std::i64;

}