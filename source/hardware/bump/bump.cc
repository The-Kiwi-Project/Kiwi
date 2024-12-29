#include "./bump.hh"
#include "../tob/tob.hh"
#include "../cob/cob.hh"
#include "../track/track.hh"
#include <cassert>
#include <numeric>

namespace kiwi::hardware {

    Bump::Bump(BumpCoord const& coord, TOB* tob) : 
        _coord{coord},
        _tob{tob},
        _connected_track{nullptr},
        _signal_dir{TOBSignalDirection::DisConnected},
        _accessable_cobunit{}
    {
        for (std::usize i = 0; i < COB::UNIT_SIZE; ++i){
            this->_accessable_cobunit.emplace(i);
        }
    }

    auto Bump::set_connected_track(Track* track, TOBSignalDirection signal_dir) -> void {
        assert(signal_dir != TOBSignalDirection::DisConnected);

        this->_connected_track = track;
        this->_signal_dir = signal_dir;

        track->set_connected_bump(this, signal_dir);
    }

    auto Bump::update_bump(Bump* bump, TOB* prev_tob, TOB* next_tob) -> Bump* {
        if (bump->_tob == prev_tob) {
            return next_tob->get_bump(bump->index()).value();
        } else {
            return bump;
        }
    }

    auto Bump::disconnect_track(Track* track) -> void {
        assert(this->_signal_dir != TOBSignalDirection::DisConnected);

        this->_connected_track = nullptr;
        this->_signal_dir = TOBSignalDirection::DisConnected;

        track->disconnect_bump(this);
    }

    auto Bump::intersect_access_unit(const std::HashSet<std::usize>& accessable_cobunit) -> void {
        std::HashSet<std::usize> intersected_cobunit;
        for (auto cobunit: accessable_cobunit) {
            if (_accessable_cobunit.contains(cobunit)){
                intersected_cobunit.emplace(cobunit);
            }
        }

        this->_accessable_cobunit = intersected_cobunit;
    }

}
