#pragma once

#include <hardware/tob/tobcoord.hh>

#include <std/collection.hh>
#include <std/string.hh>
#include <std/integer.hh>

#include <serde/de.hh>

namespace kiwi::parse {

    struct TopDieConfig {
        std::HashMap<std::String, std::usize> pin_map;
    };

    struct TopdieInstConfig {
        std::String topdie;
        hardware::TOBCoord coord;
    };

}

DESERIALIZE_STRUCT(kiwi::parse::TopDieConfig,
    DE_FILED(pin_map)
)

DESERIALIZE_STRUCT(kiwi::parse::TopdieInstConfig,
    DE_FILED(topdie)
    DE_FILED(coord)
)
