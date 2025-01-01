#include "./tbnet.hh"
#include <hardware/bump/bump.hh>


namespace kiwi::circuit {

    TrackToBumpNet::TrackToBumpNet(hardware::Track* begin_track, hardware::Bump* end_bump) :
        _begin_track{begin_track},
        _end_bump{end_bump},
        Net{Priority{3}}
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

    auto TrackToBumpNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
    }

    auto TrackToBumpNet::coords() const -> std::Vector<hardware::Coord> {
        return std::Vector<hardware::Coord>{this->begin_track()->coord(), this->end_bump()->coord()};
    }

    auto TrackToBumpNet::check_accessable_cobunit() -> void {
        std::usize index = _begin_track->coord().index;
        std::usize bank_size = hardware::COB::INDEX_SIZE/2;
        std::usize wilton_size = hardware::COBUnit::WILTON_SIZE;
        std::usize id {(index/bank_size)*wilton_size + (index%wilton_size)};
        
        std::HashSet<std::usize> cobunit_ids {id};
        _end_bump->intersect_access_unit(cobunit_ids);
    }

    
    auto TrackToBumpNet::to_string() const -> std::String {
        return std::format("Begin track: '{}' to End bump '{}'", this->_begin_track->coord(), this->_end_bump->coord());
    }

    auto TrackToBumpNet::port_number() const -> std::usize {
        return 2;
    }
}
