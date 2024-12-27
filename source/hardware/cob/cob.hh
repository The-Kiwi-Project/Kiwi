#pragma once

#include "./cobunit.hh"
#include "./cobdirection.hh"
#include "./cobregister.hh"
#include "./cobcoord.hh"
#include "../node/trackcoord.hh"

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>

namespace kiwi::hardware {

    class COBConnector {
    public:
        COBConnector(
            COBDirection from_dir, std::usize from_track_index, 
            COBDirection to_dir,   std::usize to_track_index,
            COBSwRegister&  sw_reg, COBSelRegister& t_to_c_sel_reg, COBSelRegister& c_to_t_sel_reg
        );

        auto is_connected() -> bool;
        auto connect() -> void;
        auto disconnect() -> void;

        auto from_dir() const -> COBDirection { return this->_from_dir; }
        auto from_track_index() const -> std::usize { return this->_from_track_index; }
        auto to_dir() const -> COBDirection { return this->_to_dir; }
        auto to_track_index() const -> std::usize { return this->_to_track_index; }

    private:
        COBDirection    _from_dir;
        std::usize      _from_track_index;

        COBDirection    _to_dir;
        std::usize      _to_track_index;

        COBSwRegister&  _sw_reg;

        COBSelRegister& _t_to_c_sel_reg;
        COBSelRegister& _c_to_t_sel_reg;
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
        auto sw_register(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> COBSwRegister&;
        auto get_sw_resgiter_value(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> COBSwState;
        
        auto sel_register(COBDirection dir, std::usize cob_index) -> COBSelRegister&;
        auto get_sel_resgiter_value(COBDirection dir, std::usize cob_index) -> COBSignalDirection;

    public:
        auto to_dir_track_coord(COBDirection dir, std::usize index) -> TrackCoord;
        auto cobunit_by_cob_index(std::usize cob_index) -> COBUnit&;

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
        std::Array<COBUnit, UNIT_SIZE> _cobunits;
    };

}