#include "./cobconnector.hh"
#include <std/collection.hh>
#include "hardware/cob/cobdirection.hh"
#include "hardware/cob/cobregister.hh"
#include <cassert>

namespace kiwi::hardware {

    COBConnector::COBConnector(
        COBDirection from_dir, std::usize from_track_index, 
        COBDirection to_dir, std::usize to_track_index,
        COBSwRegister*  sw_reg, 
        COBSelRegister* t_to_c_sel_reg, 
        COBSelRegister* c_to_t_sel_reg
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
        return this->_sw_reg->is_connected();
    }

    auto COBConnector::connect() -> void {
        this->_sw_reg->set(COBSwState::Connected);
        this->_t_to_c_sel_reg->set_track_to_cob();
        this->_c_to_t_sel_reg->set_cob_to_track();
    }

    auto COBConnector::disconnect() -> void {
        this->_sw_reg->set(COBSwState::DisConnected);
        this->_t_to_c_sel_reg->set_floating();
        this->_c_to_t_sel_reg->set_floating();
    }

}