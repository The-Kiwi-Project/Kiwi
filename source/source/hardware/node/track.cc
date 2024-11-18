#include "./track.hh"
#include "./bump.hh"
#include "../cob/cobcoord.hh"
#include "../cob/cobdirection.hh"

#include <std/integer.hh>
#include <std/string.hh>
#include <cassert>

namespace kiwi::hardware {

    Track::Track(const TrackCoord& coord) :
        _coord{coord},
        _prev_track{nullptr},
        _next_track{nullptr},
        _connected_bump{nullptr},
        _signal_dir{TOBSignalDirection::DisConnected}
    {
    }

    Track::Track(std::i64 r, std::i64 c, TrackDirection d, std::usize i) :
        Track{TrackCoord{r, c, d, i}}
    {
    }

    auto Track::track_path() const -> std::Vector<const Track*> {
        auto path = std::Vector<const Track*>{};

        const Track* track = this;
        while (track != nullptr) {
            path.emplace_back(track);
            track = track->next_track();
        }

        return path;
    }

    auto Track::adjacent_cob_coords() -> std::Vector<std::Tuple<COBDirection, COBCoord>> {
        auto result = std::Vector<std::Tuple<COBDirection, COBCoord>>{};

        const auto& coord = this->_coord;
        switch (coord.dir) {
            case TrackDirection::Horizontal: {
                result.emplace_back(COBDirection::Right, COBCoord{coord.row, coord.col - 1});
                result.emplace_back(COBDirection::Left, COBCoord{coord.row, coord.col});
                break;
            }
            case TrackDirection::Vertical: {
                result.emplace_back(COBDirection::Down, COBCoord{coord.row, coord.col});
                result.emplace_back(COBDirection::Up, COBCoord{coord.row - 1, coord.col});
                break;
            }
        }

        return result;
    }

    auto Track::set_connected_track(Track* pre_track) -> void {
        this->_prev_track = pre_track;
        if (pre_track != nullptr) {
            pre_track->_next_track = this;
        }
    }

    auto Track::set_connected_bump(Bump* bump, TOBSignalDirection signal_dir) -> void {
        assert(signal_dir != TOBSignalDirection::DisConnected);
        
        this->_connected_bump = bump;
        this->_signal_dir = signal_dir;
    }

}