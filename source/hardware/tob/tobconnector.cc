#include "./tobconnector.hh"

#include <std/utility.hh>
#include <std/collection.hh>
#include <debug/debug.hh>
#include <std/range.hh>
#include <std/utility.hh>
#include <cassert>

namespace kiwi::hardware {

    TOBConnector::TOBConnector(
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
    ) :
        _bump_index{bump_index},
        _hori_index{hori_index},
        _vert_index{vert_index},

        _track_index{track_inde},
        _bump_to_hori{bump_to_hori},
        _hori_to_vert{hori_to_vert},
        _vert_to_track{vert_to_track},

        _bump_dir_register{bump_dir_register},
        _track_dir_register{track_dir_register},
        _signal_dir{signal_dir}
    {
    }

    auto TOBConnector::connect() -> void {
        this->_bump_to_hori.connect();
        this->_hori_to_vert.connect();
        this->_vert_to_track.connect();

        switch (this->_signal_dir) {
            case TOBSignalDirection::BumpToTrack: {
                this->_bump_dir_register->set(TOBBumpDirection::BumpToTOB);
                this->_track_dir_register->set(TOBTrackDirection::TOBToTrack);
                break;
            }
            case TOBSignalDirection::TrackToBump: {
                this->_track_dir_register->set(TOBTrackDirection::TrackToTOB);
                this->_bump_dir_register->set(TOBBumpDirection::TOBToBump);
                break;
            }
            case TOBSignalDirection::DisConnected: {
                debug::unreachable("TOBConnector::connect()");
                break;
            }
        }
    }

    auto TOBConnector::disconnect() -> void {
        this->_bump_to_hori.disconnect();   
        this->_hori_to_vert.disconnect();   
        this->_vert_to_track.disconnect();

        this->_bump_dir_register->reset();
        this->_track_dir_register->reset();
    }

}