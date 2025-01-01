#include "./bumpcoord.hh"
#include <format>

namespace kiwi::hardware {

    BumpCoord::BumpCoord(const Coord& c, std::usize i) :
        BumpCoord{c.row, c.col, i}
    {
    }

    BumpCoord::BumpCoord(std::i64 r, std::i64 c, std::usize i) :
        Coord{r, c},
        index{i} 
    {
    }

    BumpCoord::BumpCoord() :
        BumpCoord{0, 0, 0}
    {
    }

    bool BumpCoord::operator == (const BumpCoord& other) const {
        return this->row == other.row &&
                this->col == other.col &&
                this->index == other.index;
    }

    bool BumpCoord::operator < (const BumpCoord& other) const 
    {
        return this->row > other.row ? false : \
                this->row < other.row ? true : \
                this->col > other.col ? false: \
                this->col < other.col ? true : \
                this->index >= other.index ? false : true;
    }

    auto BumpCoord::to_string() const -> std::String {
        return std::format("{}", *this);
    }

}

namespace std {

    std::size_t hash<kiwi::hardware::BumpCoord>::operator() (const kiwi::hardware::BumpCoord& coord) const noexcept {
        auto coord_hash = hash<kiwi::hardware::Coord>();
        auto usize_hash = hash<std::size_t>{};
        return coord_hash(coord) ^
               usize_hash(coord.index);
    }

}