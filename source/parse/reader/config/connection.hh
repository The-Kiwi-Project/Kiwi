#pragma once

#include <serde/de.hh>
#include <std/string.hh>

namespace kiwi::parse {

    struct ConnectionConfig {
        std::String input;
        std::String output;
    };

}

DESERIALIZE_STRUCT(kiwi::parse::ConnectionConfig,
    DE_FILED(input)
    DE_FILED(output)
)

