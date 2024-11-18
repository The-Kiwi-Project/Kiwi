#pragma once 

#include <std/string.hh>
#include <std/collection.hh>
#include <std/integer.hh>

namespace kiwi::utility {

    auto split(std::StringView origin, std::StringView delims) -> std::Vector<std::StringView>;
    auto split(std::StringView origin, char delim) -> std::Vector<std::StringView>;

    auto string_to_i32(std::StringView s) -> std::i32;

}