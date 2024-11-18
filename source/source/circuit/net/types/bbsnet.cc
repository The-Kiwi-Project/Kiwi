#include "./bbsnet.hh"
#include <hardware/node/bump.hh>


namespace kiwi::circuit {

    BumpToBumpsNet::BumpToBumpsNet(hardware::Bump* begin_bump, std::Vector<hardware::Bump*> end_bumps) :
        _begin_bump{begin_bump},
        _end_bumps{std::move(end_bumps)}
    {
    }

    BumpToBumpsNet::~BumpToBumpsNet() noexcept {

    }

    auto BumpToBumpsNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        this->_begin_bump = hardware::Bump::update_bump(this->_begin_bump, prev_tob, next_tob);
        for (auto& bump : this->_end_bumps) {
            bump = hardware::Bump::update_bump(bump, prev_tob, next_tob);
        }
    }

    auto BumpToBumpsNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> void {
        // return strategy.route_bump_to_bumps_net(interposer, this);
    }

    auto BumpToBumpsNet::priority() const -> Priority {
        return {1};
    }

    auto BumpToBumpsNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto bump : this->end_bumps()) {
            coords.emplace_back(bump->coord());
        }
        coords.emplace_back(this->begin_bump()->coord());
        return coords;
    }
    
}
