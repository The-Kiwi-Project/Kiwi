#include "./tbsnet.hh"
#include <hardware/node/bump.hh>



namespace kiwi::circuit {

    TrackToBumpsNet::TrackToBumpsNet(hardware::Track* begin_track, std::Vector<hardware::Bump*> end_bumps) :
        _begin_track{begin_track},
        _end_bumps{std::move(end_bumps)}
    {
    }

    TrackToBumpsNet::~TrackToBumpsNet() noexcept {

    }

    auto TrackToBumpsNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        for (auto& bump : this->_end_bumps) {
            bump = hardware::Bump::update_bump(bump, prev_tob, next_tob);
        }
    }

    auto TrackToBumpsNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_track_to_bumps_net(interposer, this);
    }

    auto TrackToBumpsNet::priority() const -> Priority {
        return {1};
    }

    auto TrackToBumpsNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto bump : this->end_bumps()) {
            coords.emplace_back(bump->coord());
        }
        coords.emplace_back(this->begin_track()->coord());
        return coords;
    }

    auto TrackToBumpsNet::check_accessable_cobunit() -> void {
        std::usize index = _begin_track->coord().index;
        std::usize bank_size = hardware::COB::INDEX_SIZE/2;
        std::usize wilton_size = hardware::COBUnit::WILTON_SIZE;
        std::usize id {(index/bank_size)*wilton_size + (index%wilton_size)};
        
        std::HashSet<std::usize> cobunit_ids {id};
        for (auto bump: _end_bumps){
            bump->intersect_access_unit(cobunit_ids);
        }
    }

}
