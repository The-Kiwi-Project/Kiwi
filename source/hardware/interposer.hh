#pragma once

#include "./cob/cobcoord.hh"
#include "./cob/cob.hh"

#include "./tob/tobcoord.hh"
#include "./tob/tob.hh"

#include "./track/track.hh"
#include "./bump/bump.hh"
#include "hardware/coord.hh"
#include "std/memory.hh"
#include <std/integer.hh>

#include <std/utility.hh>
#include <std/collection.hh>

namespace kiwi::hardware {

    class Interposer {
    public:
        enum {
            // total columns = 12
            COB_ARRAY_WIDTH   = 12,
            COB_ARRAY_HEIGHT  = 9,

            TOB_ARRAY_WIDTH   = 4,
            TOB_ARRAY_HEIGHT  = 4,
        };

        static const std::HashMap<Coord, Coord> TOB_COORD_MAP;

    public:
        Interposer();

    public:
        auto available_tracks(Bump* bump, TOBSignalDirection dir) -> std::HashMap<Track*, TOBConnector>;
        auto available_tracks_bump_to_track(Bump* bump) -> std::HashMap<Track*, TOBConnector>;
        auto available_tracks_track_to_bump(Bump* bump) -> std::HashMap<Track*, TOBConnector>;

    public:
        auto adjacent_idle_tracks(Track* track) -> std::Vector<std::Tuple<Track*, COBConnector>>;
        auto adjacent_tracks(Track* track) -> std::Vector<std::Tuple<Track*, COBConnector>>;

    public:
        auto is_idle_tracks(Track* track) -> bool;

    public:
        auto get_cob(const COBCoord& coord) -> std::Option<COB*>;
        auto get_cob(std::i64 row, std::i64 col) -> std::Option<COB*>;
         
        auto get_tob(const TOBCoord& coord) -> std::Option<TOB*>;
        auto get_tob(std::i64 row, std::i64 col) -> std::Option<TOB*>;

        auto get_track(const TrackCoord& coord) -> std::Option<Track*>;
        auto get_track(std::i64 row, std::i64 col, TrackDirection dir, std::usize index) -> std::Option<Track*>;

        auto get_bump(const TOBCoord& coord, std::usize index) -> std::Option<Bump*>;
        auto get_bump(std::i64 row, std::i64 col, std::usize index) -> std::Option<Bump*>;

        auto get_a_idle_tob() -> std::Option<TOB*>;
        
    public:
        auto randomly_map_remain_indexes() -> void;
        auto manage_cobunit_resources() -> void;

    private:
        auto static check_track_coord(const TrackCoord& coord) -> bool;
    
    private:
        // Maybe, just array...
        std::HashMap<COBCoord, std::Box<COB>> _cobs;
        std::HashMap<TOBCoord, std::Box<TOB>> _tobs;
        std::HashMap<TrackCoord, std::Box<Track>> _tracks;
    };

}