
#pragma once

#include "../coord.hh"
#include "serde/de.hh"

#include <std/integer.hh>
#include <std/string.hh>
#include <std/format.hh>

namespace kiwi::hardware {

    enum class TrackDirection {
        Horizontal,
        Vertical,
    };

    struct TrackCoord : public Coord {
        TrackCoord(std::i64 r, std::i64 c, TrackDirection d, std::usize i);
        TrackCoord();
                
        bool operator == (const TrackCoord& other) const;
        bool operator < (const TrackCoord& other) const;
        auto to_string() const -> std::String;

        TrackDirection dir;
        std::usize index;
    };

}

FORMAT_ENUM(kiwi::hardware::TrackDirection,
    kiwi::hardware::TrackDirection::Horizontal,
    kiwi::hardware::TrackDirection::Vertical
)

FORMAT_STRUCT(kiwi::hardware::TrackCoord, row, col, dir, index)

DESERIALIZE_ENUM(kiwi::hardware::TrackDirection,
    DE_VALUE_AS(kiwi::hardware::TrackDirection::Horizontal, "h")
    DE_VALUE_AS(kiwi::hardware::TrackDirection::Vertical, "v")
)

DESERIALIZE_STRUCT(kiwi::hardware::TrackCoord, 
    DE_FILED(row)
    DE_FILED(col)
    DE_FILED(dir)
    DE_FILED(index)
)

template <>
struct std::hash<kiwi::hardware::TrackDirection> {
    std::size_t operator() (const kiwi::hardware::TrackDirection& dir) const noexcept;
};

template <>
struct std::hash<kiwi::hardware::TrackCoord> {
    std::size_t operator() (const kiwi::hardware::TrackCoord& coord) const noexcept;
};
