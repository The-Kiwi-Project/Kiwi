#include "./bbnet.hh"
#include "std/string.hh"
#include <std/format.hh>
#include <hardware/bump/bump.hh>


namespace kiwi::circuit {

    BumpToBumpNet::BumpToBumpNet(hardware::Bump* begin_bump, hardware::Bump* end_bump) :
        _begin_bump{begin_bump},
        _end_bump{end_bump},
        Net{Priority{4}}
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

    auto BumpToBumpNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
    }

    auto BumpToBumpNet::coords() const -> std::Vector<hardware::Coord> {
        return std::Vector<hardware::Coord>{this->begin_bump()->coord(), this->end_bump()->coord()};
    }

    auto BumpToBumpNet::check_accessable_cobunit() -> void {
        std::HashSet<std::usize> accessable_cobunit {};
        for (std::usize i = 0; i < hardware::COB::UNIT_SIZE; ++i){
            accessable_cobunit.emplace(i);
        }
        _begin_bump->intersect_access_unit(accessable_cobunit);
        _end_bump->intersect_access_unit(accessable_cobunit);
    }

    auto BumpToBumpNet::port_number() const -> std::usize {
        return 2;
    }

    auto BumpToBumpNet::to_string() const -> std::String {
        return std::format("Begin bump: '{}' to End bump '{}'", this->_begin_bump->coord(), this->_end_bump->coord());
    }

}
