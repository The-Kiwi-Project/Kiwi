#include "./trackcoord.hh"

namespace kiwi::hardware {

    TrackCoord::TrackCoord(std::i64 r, std::i64 c, TrackDirection d, std::usize i) :
        Coord{r, c},
        dir{d}, index{i} 
    {
    }

    TrackCoord::TrackCoord() :
        TrackCoord{0, 0, TrackDirection::Vertical, 0}
    {
    }

    bool TrackCoord::operator == (const TrackCoord& other) const {
        return this->row == other.row &&
                this->col == other.col &&
                this->dir == other.dir &&
                this->index == other.index;
    }

    bool TrackCoord::operator < (const TrackCoord& other) const 
    {
        return this->row > other.row ? false : \
                this->row < other.row ? true : \
                this->col > other.col ? false: \
                this->col < other.col ? true : \
                this->index >= other.index ? false : true;
    }

    auto TrackCoord::to_string() const -> std::String {
        return std::format("{}", *this);
    }

}

namespace std {

    std::size_t hash<kiwi::hardware::TrackDirection>::operator() (const kiwi::hardware::TrackDirection& dir) const noexcept {
        if (dir == kiwi::hardware::TrackDirection::Horizontal) {
            return 0xCCCCCCCCCCCCCCCCULL;
        } else {
            return 0x3333333333333333ULL;
        }
    }

    std::size_t hash<kiwi::hardware::TrackCoord>::operator() (const kiwi::hardware::TrackCoord& coord) const noexcept {
        auto coord_hash = hash<kiwi::hardware::Coord>();
        auto usize_hash = hash<std::size_t>{};
        auto dir_hash = hash<kiwi::hardware::TrackDirection>{};
        return coord_hash(coord) ^
                dir_hash(coord.dir) ^
                usize_hash(coord.index);
    }

}