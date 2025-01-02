#pragma once

#include "./cobunit.hh"
#include "./cobdirection.hh"
#include "./cobregister.hh"
#include "./cobcoord.hh"
#include "../track/trackcoord.hh"
#include "./cobconnector.hh"

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>

namespace kiwi::hardware {

    struct TrackIndex {
        explicit TrackIndex(std::usize i) : value{i} {}
        std::usize value;
    };

    struct COBIndex {
        explicit COBIndex(std::usize i) : value{i} {}
        std::usize value;
    };

    class COB {
    public:
        enum { 
            UNIT_SIZE = 16,
            INDEX_SIZE = UNIT_SIZE * COBUnit::WILTON_SIZE
        };

    public: 
        COB(const COBCoord& coord);
        COB(std::i64 row, std::i64 col);

    public:
        auto adjacent_connectors(COBDirection from_dir, std::usize from_track_index) -> std::Vector<COBConnector>;
        
        auto adjacent_connectors_from_left(std::usize from_track_index) -> std::Vector<COBConnector>;
        auto adjacent_connectors_from_right(std::usize from_track_index) -> std::Vector<COBConnector>;
        auto adjacent_connectors_from_up(std::usize from_track_index) -> std::Vector<COBConnector>;
        auto adjacent_connectors_from_down(std::usize from_track_index) -> std::Vector<COBConnector>;

    public:
        auto sw_register(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> COBSwRegister*;
        auto sw_register(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) const -> const COBSwRegister*;

        auto sel_register(COBDirection dir, std::usize cob_index) -> COBSelRegister*;
        auto sel_register(COBDirection dir, std::usize cob_index) const -> const COBSelRegister*;

        auto get_sw_resgiter_value(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) const -> COBSwState;
        auto get_sel_resgiter_value(COBDirection dir, std::usize cob_index) const -> COBSignalDirection;

    // public:
    //     auto sw_register(COBDirection from_dir, TrackIndex from_index, COBDirection to_dir) -> COBSwRegister*;
    //     auto sw_register(COBDirection from_dir, TrackIndex from_index, COBDirection to_dir) const -> const COBSwRegister*;

    //     auto sel_register(COBDirection dir, TrackIndex index) -> COBSelRegister*;
    //     auto sel_register(COBDirection dir, TrackIndex index) const -> const COBSelRegister*;

    //     auto get_sw_resgiter_value(COBDirection from_dir, TrackIndex from_index, COBDirection to_dir) const -> COBSwState;
    //     auto get_sel_resgiter_value(COBDirection dir, TrackIndex index) const -> COBSignalDirection;

    public:
        auto to_dir_track_coord(COBDirection dir, std::usize index) -> TrackCoord;

    public:
        static auto track_index_map(COBDirection from_dir, std::usize from_track_index, COBDirection to_dir) -> std::usize;
        static auto cob_index_map(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> std::usize;

    public:
        static auto cob_index_to_track_index(std::usize cob_index) -> std::usize;
        static auto track_index_to_cob_index(std::usize track_index) -> std::usize;

    public:
        static auto cob_index_to_cobunit_info(COBDirection dir, std::usize cob_index) -> std::Tuple<std::usize, std::usize>;
        static auto cobunit_info_to_cob_index(COBDirection dir, std::Tuple<std::usize, std::usize> cobunit_info) -> std::usize;

        static auto assert_index(std::usize index) -> void;

    public:
        auto coord() const -> const COBCoord& { return this->_coord; }

    private:
        COBCoord _coord;
        std::Vector<std::Box<COBUnit>> _cobunits;
    };

}