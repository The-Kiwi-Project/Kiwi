#include "./cob.hh"
#include <std/collection.hh>
#include "debug/debug.hh"
#include "hardware/cob/cobcoord.hh"
#include "hardware/cob/cobdirection.hh"
#include "hardware/cob/cobregister.hh"
#include "hardware/cob/cobunit.hh"
#include <cassert>

namespace kiwi::hardware {

    COBConnector::COBConnector(
        COBDirection from_dir, std::usize from_track_index, 
        COBDirection to_dir, std::usize to_track_index,
        COBSwRegister&  sw_reg, COBSelRegister& t_to_c_sel_reg, COBSelRegister& c_to_t_sel_reg
    ) :
        _from_dir{from_dir},
        _from_track_index{from_track_index},
        _to_dir{to_dir},
        _to_track_index{to_track_index},
        _sw_reg{sw_reg},
        _t_to_c_sel_reg{t_to_c_sel_reg},
        _c_to_t_sel_reg{c_to_t_sel_reg}
    {}

    auto COBConnector::is_connected() -> bool {
        return this->_sw_reg;
    }

    auto COBConnector::connect() -> void {
        this->_sw_reg.set(COBSwState::Connected);
        this->_t_to_c_sel_reg.set_track_to_cob();
        this->_c_to_t_sel_reg.set_cob_to_track();
    }

    auto COBConnector::disconnect() -> void {
        this->_sw_reg.set(COBSwState::DisConnected);
        this->_t_to_c_sel_reg.set_floating();
        this->_c_to_t_sel_reg.set_floating();
    }

    //////////////////////////////////////////////////////

    COB::COB(const COBCoord& coord) :
        _coord{coord},
        _cobunits{}
    {
    }

    COB::COB(std::i64 row, std::i64 col) : 
        COB{COBCoord{row, col}}
    {
    }


    auto COB::adjacent_connectors(COBDirection from_dir, std::usize from_track_index) -> std::Vector<COBConnector> {
        COB::assert_index(from_track_index);
        
        auto from_cob_index = COB::track_index_to_cob_index(from_track_index);
        
        auto [cobunit_index, from_cobunit_inner_index] = 
            COB::cob_index_to_cobunit_info(from_dir, from_cob_index);
        COBUnit& cobunit = this->_cobunits[cobunit_index];

        auto connectors = std::Vector<COBConnector>{};
        for (auto& unit_ctr : cobunit.adjacent_connectors(from_dir, from_cobunit_inner_index)) {
            auto to_cob_index = COB::cobunit_info_to_cob_index(
                unit_ctr.to_dir,
                {cobunit_index, unit_ctr.to_index}
            );
            auto to_track_index = COB::cob_index_to_track_index(to_cob_index);

            connectors.emplace_back(
                unit_ctr.from_dir,
                from_track_index,
                unit_ctr.to_dir,
                to_track_index,
                unit_ctr.sw_reg,
                unit_ctr.t_to_c_sel_reg,
                unit_ctr.c_to_t_sel_reg
            );
        }

        return connectors;
    }

    auto COB::adjacent_connectors_from_left(std::usize from_track_index) -> std::Vector<COBConnector> {
        return this->adjacent_connectors(COBDirection::Left, from_track_index);
    }

    auto COB::adjacent_connectors_from_right(std::usize from_track_index) -> std::Vector<COBConnector> {
        return this->adjacent_connectors(COBDirection::Right, from_track_index);
    }

    auto COB::adjacent_connectors_from_up(std::usize from_track_index) -> std::Vector<COBConnector> {
        return this->adjacent_connectors(COBDirection::Up, from_track_index);
    }

    auto COB::adjacent_connectors_from_down(std::usize from_track_index) -> std::Vector<COBConnector> {
        return this->adjacent_connectors(COBDirection::Down, from_track_index);
    }

    auto COB::to_dir_track_coord(COBDirection dir, std::usize index) -> TrackCoord {
        const auto& coord = this->_coord;
        switch (dir) {
            case COBDirection::Left: return TrackCoord{coord.row, coord.col, TrackDirection::Horizontal, index};
            case COBDirection::Right: return TrackCoord{coord.row, coord.col + 1, TrackDirection::Horizontal, index};
            case COBDirection::Up: return TrackCoord{coord.row + 1, coord.col, TrackDirection::Vertical, index};
            case COBDirection::Down: return TrackCoord{coord.row, coord.col, TrackDirection::Vertical, index};
        }
        debug::unreachable();
    }

    auto COB::track_index_map(COBDirection from_dir, std::usize from_track_index, COBDirection to_dir) -> std::usize {
        COB::assert_index(from_track_index);
    
        auto from_cob_index = COB::track_index_to_cob_index(from_track_index);
        auto to_cob_index = COB::cob_index_map(from_dir, from_cob_index, to_dir);
        return COB::cob_index_to_track_index(to_cob_index);
    }

    auto COB::cob_index_map(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> std::usize {
        COB::assert_index(from_cob_index);
        
        auto [cobunit_index, from_cobunit_inner_index] = COB::cob_index_to_cobunit_info(from_dir, from_cob_index);
        auto to_cobunit_inner_index = COBUnit::index_map(from_dir, from_cobunit_inner_index, to_dir);
        return COB::cobunit_info_to_cob_index(to_dir, {cobunit_index, to_cobunit_inner_index});
    }

    auto COB::cob_index_to_track_index(std::usize cob_index) -> std::usize {
        COB::assert_index(cob_index);

        if (cob_index < 64) {
            auto groups_index = cob_index % 8;
            auto group_index = cob_index / 8;
            return 8 * groups_index + group_index;
        } else {
            cob_index = cob_index - 64;
            auto groups_index = cob_index % 8;
            auto group_index = cob_index / 8;
            return 8 * groups_index + group_index + 64;
        }
    }

    auto COB::track_index_to_cob_index(std::usize track_index) -> std::usize {
        COB::assert_index(track_index);

        if (track_index < 64) {
            auto groups_index = track_index % 8;
            auto group_index = track_index / 8;
            return 8 * groups_index + group_index;
        } else {
            track_index = track_index - 64;
            auto groups_index = track_index % 8;
            auto group_index = track_index / 8;
            return 8 * groups_index + group_index + 64;
        }
    }

    auto COB::cob_index_to_cobunit_info(COBDirection dir, std::usize cob_index) -> std::Tuple<std::usize, std::usize> {
        COB::assert_index(cob_index);
        return {
            cob_index / COBUnit::WILTON_SIZE,
            cob_index % COBUnit::WILTON_SIZE,
        };
    }

    auto COB::cobunit_info_to_cob_index(COBDirection dir, std::Tuple<std::usize, std::usize> cobunit_info) -> std::usize {
        auto [cobunit_index, cobunit_inner_index] = cobunit_info;
        
        COB::assert_index(cobunit_inner_index);
        assert(cobunit_index < COB::UNIT_SIZE);
        
        return cobunit_index * COBUnit::WILTON_SIZE + cobunit_inner_index; 
    }

    auto COB::assert_index(std::usize index) -> void {
        assert(index < COB::INDEX_SIZE);
    }

    auto COB::sw_register(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> COBSwRegister& {
        auto [group, group_index] = cob_index_to_cobunit_info(from_dir, from_cob_index);
        auto& cob_unit = this->_cobunits[group];
        return cob_unit.sw_register(from_dir, group_index, to_dir);
    }

    auto COB::get_sw_resgiter_value(COBDirection from_dir, std::usize from_cob_index, COBDirection to_dir) -> COBSwState {
        auto& cob_swregister = sw_register(from_dir, from_cob_index, to_dir);
        return cob_swregister.get();
    }

    auto COB::sel_register(COBDirection dir, std::usize cob_index) -> COBSelRegister& {
        auto [group, group_index] = cob_index_to_cobunit_info(dir, cob_index);
        auto& cob_unit = this->_cobunits[group];
        return cob_unit.sel_register(dir, group_index);
    }

    auto COB::get_sel_resgiter_value(COBDirection dir, std::usize cob_index) -> COBSignalDirection {
        auto& cob_selregister = sel_register(dir, cob_index);
        return cob_selregister.get();
    }

}