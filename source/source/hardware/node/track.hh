#pragma once

#include "./trackcoord.hh"
#include "../cob/cobcoord.hh"
#include "../cob/cobdirection.hh"
#include "../tob/tobsigdir.hh"

#include <std/integer.hh>
#include <std/collection.hh>
#include <std/utility.hh>

namespace kiwi::hardware {

    class Bump;

    class Track {
    public:
        Track(const TrackCoord& coord);
        Track(std::i64 r, std::i64 c, TrackDirection d, std::usize i);

    public:
        auto track_path() const -> std::Vector<const Track*>;

    public:
        auto adjacent_cob_coords() -> std::Vector<std::Tuple<COBDirection, COBCoord>>;

    public:
        // this->next ==> pre_track, pre_track->prev = this
        auto set_connected_track(Track* pre_track) -> void;
        auto set_connected_bump(Bump* bump, TOBSignalDirection signal_dir) -> void;

        auto coord() const -> const TrackCoord& { return this->_coord; } 
        auto prev_track() const -> Track* { return this->_prev_track; }
        auto next_track() const -> Track* { return this->_next_track; }

    private:
        TrackCoord _coord;
        
        Track* _prev_track;
        Track* _next_track;

        Bump* _connected_bump;
        TOBSignalDirection _signal_dir;
    };

}
