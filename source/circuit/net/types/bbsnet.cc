#include "./bbsnet.hh"
#include <hardware/node/bump.hh>


namespace kiwi::circuit {

    BumpToBumpsNet::BumpToBumpsNet(hardware::Bump* begin_bump, std::Vector<hardware::Bump*> end_bumps) :
        _begin_bump{begin_bump},
        _end_bumps{std::move(end_bumps)},
        Net{Priority{2}}
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

    auto BumpToBumpsNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_bump_to_bumps_net(interposer, this);
    }

    auto BumpToBumpsNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
    }

    auto BumpToBumpsNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto bump : this->end_bumps()) {
            coords.emplace_back(bump->coord());
        }
        coords.emplace_back(this->begin_bump()->coord());
        return coords;
    }

    auto BumpToBumpsNet::check_accessable_cobunit() -> void {
        std::HashSet<std::usize> accessable_cobunit {};
        for (std::usize i = 0; i < hardware::COB::UNIT_SIZE; ++i){
            accessable_cobunit.emplace(i);
        }
        _begin_bump->intersect_access_unit(accessable_cobunit);
        for (auto bump : _end_bumps) {
            bump->intersect_access_unit(accessable_cobunit);
        }
    }

    auto BumpToBumpsNet::port_number() const -> std::usize {
        return (this->_end_bumps.size() + 1);
    }
    
}
