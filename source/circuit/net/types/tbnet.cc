#include "./tbnet.hh"
#include <hardware/node/bump.hh>


namespace kiwi::circuit {

    TrackToBumpNet::TrackToBumpNet(hardware::Track* begin_track, hardware::Bump* end_bump) :
        _begin_track{begin_track},
        _end_bump{end_bump}
    {
    }

    TrackToBumpNet::~TrackToBumpNet() noexcept {
    }

    auto TrackToBumpNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        this->_end_bump = hardware::Bump::update_bump(this->_end_bump, prev_tob, next_tob);
    }

    auto TrackToBumpNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_track_to_bump_net(interposer, this);
    }

    auto TrackToBumpNet::priority() const -> Priority {
        return {3};
    }

    auto TrackToBumpNet::coords() const -> std::Vector<hardware::Coord> {
        return std::Vector<hardware::Coord>{this->begin_track()->coord(), this->end_bump()->coord()};
    }

}
