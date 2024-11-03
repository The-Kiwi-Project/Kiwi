#include "./btsnet.hh"
#include <hardware/node/bump.hh>


namespace kiwi::circuit {

    BumpToTracksNet::BumpToTracksNet(hardware::Bump* begin_bump, std::Vector<hardware::Track*> end_tracks) :
        _begin_bump{begin_bump},
        _end_tracks{std::move(end_tracks)}
    {
    }

    BumpToTracksNet::~BumpToTracksNet() noexcept {

    }

    auto BumpToTracksNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        this->_begin_bump = hardware::Bump::update_bump(this->_begin_bump, prev_tob, next_tob);
    }

    auto BumpToTracksNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> void {
        // return strategy.route_bump_to_tracks_net(interposer, this);
    }

    auto BumpToTracksNet::priority() const -> Priority {
        return {1};
    }

    auto BumpToTracksNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto track : this->end_tracks()) {
            coords.emplace_back(track->coord());
        }
        coords.emplace_back(this->begin_bump()->coord());
        return coords;
    }
    
}
