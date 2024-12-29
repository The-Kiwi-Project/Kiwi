#pragma once

#include "./cobdirection.hh"
#include "./cobregister.hh"

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>

namespace kiwi::hardware {

    class COBConnector {
    public:
        COBConnector(
            COBDirection from_dir, std::usize from_track_index, 
            COBDirection to_dir,   std::usize to_track_index,
            COBSwRegister*  sw_reg, 
            COBSelRegister* t_to_c_sel_reg, 
            COBSelRegister* c_to_t_sel_reg
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

        COBSwRegister*  _sw_reg;
        COBSelRegister* _t_to_c_sel_reg;
        COBSelRegister* _c_to_t_sel_reg;
    };

}