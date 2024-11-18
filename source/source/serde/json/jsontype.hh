#pragma once

#include <std/string.hh>
#include <std/format.hh>

namespace kiwi::serde {

    enum class JsonType {
        Null    = 0,
        Boolean = 1, 
        Integer = 2,
        Decimal = 3,
        String  = 4,
        Array   = 5,
        Object  = 6,
    };

    auto jsonTypeToString(JsonType type) -> std::StringView;

}