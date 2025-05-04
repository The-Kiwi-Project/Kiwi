#include "./tbsnet.hh"
#include <hardware/bump/bump.hh>



namespace kiwi::circuit {

    TrackToBumpsNet::TrackToBumpsNet(hardware::Track* begin_track, std::Vector<hardware::Bump*> end_bumps) :
        _begin_track{begin_track},
        _end_bumps{std::move(end_bumps)},
        Net{Priority{1}}
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

    auto TrackToBumpsNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
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

    auto TrackToBumpsNet::to_string() const -> std::String {
        auto ss = std::StringStream {};
        ss << std::format("Begin track '{}' to End bumps '[", this->_begin_track->coord());
        for (int i = 0; i < this->_end_bumps.size(); ++i) {
            if (i != 0) {
                ss << ", ";
            }
            ss << std::format("{}", this->_end_bumps[i]->coord());
        }
        ss << ']';
        return ss.str();
    }

    auto TrackToBumpsNet::port_number() const -> std::usize {
        return (this->_end_bumps.size() + 1);
    }

    auto TrackToBumpsNet::has_tob_in_ports(hardware::TOB* tob) const -> bool {
        for (auto& b: this->_end_bumps) {
            if (b->tob()->coord() == tob->coord()) {
                return true;
            }
        }
        return false;
    }

}
