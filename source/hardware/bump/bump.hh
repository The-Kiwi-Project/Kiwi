#pragma once

#include "../tob/tobsigdir.hh"
#include "./bumpcoord.hh"

#include <std/integer.hh>
#include <std/collection.hh>

namespace kiwi::hardware {

    class Track;
    class TOB;
    class COB;
    class COBUnit;

    class Bump {
    public:
        Bump(BumpCoord const& coord, TOB* tob);

    public:
        static auto update_bump(Bump* bump, TOB* prev_tob, TOB* next_tob) -> Bump*;
        auto set_connected_track(Track* track, TOBSignalDirection signal_dir) -> void;
        auto disconnect_track(Track* track) -> void;
        auto intersect_access_unit(const std::HashSet<std::usize>& accessable_cobunit) -> void;

    public:
        auto coord() const -> BumpCoord const& { return this->_coord; } 
        auto index() const -> std::usize { return this->_coord.index; }
        auto tob() const -> TOB* { return this->_tob; }

        auto connected_track() const -> Track* { return this->_connected_track; }
        auto signal_dir() const -> TOBSignalDirection { return this->_signal_dir; }
        auto accessable_cobunit() const -> const std::HashSet<std::usize>& { return this->_accessable_cobunit; }

    private:
        BumpCoord const _coord;
        TOB* const _tob;

        Track* _connected_track; 
        TOBSignalDirection _signal_dir;
        std::HashSet<std::usize> _accessable_cobunit;   // at least within the range
    };

}
