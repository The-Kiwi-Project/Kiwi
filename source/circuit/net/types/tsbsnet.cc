#include "./tsbsnet.hh"
#include <hardware/node/bump.hh>


namespace kiwi::circuit {

    TracksToBumpsNet::TracksToBumpsNet(std::Vector<hardware::Track*> begin_tracks, std::Vector<hardware::Bump*>  end_bumps):
        _begin_tracks{begin_tracks}, _end_bumps{end_bumps}
    {

    }

    auto TracksToBumpsNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        for (auto& bump : this->_end_bumps) {
            bump = hardware::Bump::update_bump(bump, prev_tob, next_tob);
        }
    }

    auto TracksToBumpsNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_tracks_to_bumps_net(interposer, this);
    }

    auto TracksToBumpsNet::priority() const -> Priority {
        return {3};
    }

    auto TracksToBumpsNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto track : this->begin_tracks()) {
            coords.emplace_back(track->coord());
        }
        for (auto bump : this->end_bumps()) {
            coords.emplace_back(bump->coord());
        }
        return coords;
    }

}
