#pragma once

#include "serde/de.hh"
#include <hardware/track/trackcoord.hh>

namespace kiwi::parse {

    struct ExternalPortConfig {
        hardware::TrackCoord coord;
    };
    
}

DESERIALIZE_STRUCT(kiwi::parse::ExternalPortConfig,
    DE_FILED(coord)
)