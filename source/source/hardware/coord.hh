#pragma once 

#include <std/integer.hh>
#include <std/format.hh>
#include <serde/de.hh>

namespace kiwi::hardware {

    struct Coord {
        Coord(std::i64 row, std::i64 col);
        Coord();
        
        bool operator == (const Coord& other) const;

        std::i64 row;
        std::i64 col;
    };

}

DESERIALIZE_STRUCT(kiwi::hardware::Coord,
    DE_FILED(row)
    DE_FILED(col)
)

FORMAT_STRUCT(kiwi::hardware::Coord, row, col)

template<>
struct std::hash<kiwi::hardware::Coord> {
    size_t operator() (const kiwi::hardware::Coord& c) const noexcept;
};
