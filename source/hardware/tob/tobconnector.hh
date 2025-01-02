#pragma once 

#include "./tobmux.hh"
#include "./tobsigdir.hh"

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>
#include <hardware/tob/tobcoord.hh>
#include <hardware/tob/tobregister.hh>

namespace kiwi::hardware {

    class TOBConnector {
    public:
        TOBConnector(
            std::usize bump_index, 
            std::usize hori_index, 
            std::usize vert_index, 
            std::usize track_inde,

            TOBMuxConnector bump_to_hori,
            TOBMuxConnector hori_to_vert,
            TOBMuxConnector vert_to_track,
            
            TOBBumpDirRegister* bump_dir_register,
            TOBTrackDirRegister* track_dir_register,
            TOBSignalDirection signal_dir
        );

        auto connect() -> void;
        auto disconnect() -> void;

        auto bump_index() const -> std::usize { return this->_bump_index; }
        auto hori_index() const -> std::usize { return this->_hori_index; }
        auto vert_index() const -> std::usize { return this->_vert_index; }
        auto track_index() const -> std::usize { return this->_track_index; }
        auto single_direction() const -> TOBSignalDirection { return this->_signal_dir; }

    private:
        std::usize _bump_index;
        std::usize _hori_index;
        std::usize _vert_index;
        std::usize _track_index;
    
        TOBMuxConnector _bump_to_hori;
        TOBMuxConnector _hori_to_vert;
        TOBMuxConnector _vert_to_track;

        TOBBumpDirRegister* _bump_dir_register;
        TOBTrackDirRegister* _track_dir_register;
        TOBSignalDirection _signal_dir;
    };

}