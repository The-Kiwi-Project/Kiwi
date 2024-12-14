#include "./bbnet.hh"
#include "debug/debug.hh"
#include <hardware/node/bump.hh>


namespace kiwi::circuit {

    BumpToBumpNet::BumpToBumpNet(hardware::Bump* begin_bump, hardware::Bump* end_bump) :
        _begin_bump{begin_bump},
        _end_bump{end_bump}
    {
    }

    BumpToBumpNet::~BumpToBumpNet() noexcept {
    }

    auto BumpToBumpNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        this->_begin_bump = hardware::Bump::update_bump(this->_begin_bump, prev_tob, next_tob);
        this->_end_bump = hardware::Bump::update_bump(this->_end_bump, prev_tob, next_tob);
    }

    auto BumpToBumpNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_bump_to_bump_net(interposer, this);
    }

    auto BumpToBumpNet::priority() const -> Priority {
        return {2};
    }

    auto BumpToBumpNet::coords() const -> std::Vector<hardware::Coord> {
        return std::Vector<hardware::Coord>{this->begin_bump()->coord(), this->end_bump()->coord()};
    }

}
