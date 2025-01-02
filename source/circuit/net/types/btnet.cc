#include "./btnet.hh"
#include <hardware/bump/bump.hh>


namespace kiwi::circuit {

    BumpToTrackNet::BumpToTrackNet(hardware::Bump* begin_bump, hardware::Track* end_track) :
        _begin_bump{begin_bump},
        _end_track{end_track},
        Net{Priority{3}}
    {
    }

    BumpToTrackNet::~BumpToTrackNet() noexcept {
    }

    auto BumpToTrackNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        this->_begin_bump = hardware::Bump::update_bump(this->_begin_bump, prev_tob, next_tob);
    }

    auto BumpToTrackNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_bump_to_track_net(interposer, this);
    }

    auto BumpToTrackNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
    }

    auto BumpToTrackNet::coords() const -> std::Vector<hardware::Coord> {
        return std::Vector<hardware::Coord>{this->begin_bump()->coord(), this->end_track()->coord()};
    }

    auto BumpToTrackNet::check_accessable_cobunit() -> void {
        std::usize index = _end_track->coord().index;
        std::usize bank_size = hardware::COB::INDEX_SIZE/2;
        std::usize wilton_size = hardware::COBUnit::WILTON_SIZE;
        std::usize id {(index/bank_size)*wilton_size + (index%wilton_size)};
        
        std::HashSet<std::usize> cobunit_ids {id};
        _begin_bump->intersect_access_unit(cobunit_ids);
    }

    auto BumpToTrackNet::to_string() const -> std::String {
        return std::format("Begin bump: '{}' to End track '{}'", this->_begin_bump->coord(), this->_end_track->coord());
    }

    auto BumpToTrackNet::port_number() const -> std::usize {
        return 2;
    }
}
