#include "./tsbsnet.hh"
#include <hardware/bump/bump.hh>


namespace kiwi::circuit {

    TracksToBumpsNet::TracksToBumpsNet(std::Vector<hardware::Track*> begin_tracks, std::Vector<hardware::Bump*>  end_bumps):
        _begin_tracks{begin_tracks}, _end_bumps{end_bumps}, Net{Priority{5}}
    {

    }

    auto TracksToBumpsNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        for (auto& bump : this->_end_bumps) {
            bump = hardware::Bump::update_bump(bump, prev_tob, next_tob);
        }
    }

    auto TracksToBumpsNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_tracks_to_bumps_net(interposer, this);
    }

    auto TracksToBumpsNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
    }

    auto TracksToBumpsNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto track : this->begin_tracks()) {
            coords.emplace_back(track->coord());
        }
        for (auto bump : this->end_bumps()) {
            coords.emplace_back(bump->coord());
        }
        return coords;
    }

    auto TracksToBumpsNet::check_accessable_cobunit() -> void {
        std::HashSet<std::usize> accessable_cobunit {};
        std::usize bank_size = hardware::COB::INDEX_SIZE/2;
        std::usize wilton_size = hardware::COBUnit::WILTON_SIZE;

        for (auto track: _begin_tracks){
            std::usize index = track->coord().index;
            std::usize id {(index/bank_size)*wilton_size + (index%wilton_size)};
            accessable_cobunit.emplace(id);
        }
        for (auto bump: _end_bumps){
            bump->intersect_access_unit(accessable_cobunit);
        }
    }

    auto TracksToBumpsNet::to_string() -> std::String {
        auto ss = std::StringStream {};
        ss << "Begin tracks '[";
        for (int i = 0; i < this->_begin_tracks.size(); ++i) {
            if (i != 0) {
                ss << ", ";
            }
            ss << std::format("{}", this->_end_bumps[i]->coord());
        }
        ss << "] to End bumps '[";
        for (int i = 0; i < this->_end_bumps.size(); ++i) {
            if (i != 0) {
                ss << ", ";
            }
            ss << std::format("{}", this->_end_bumps[i]->coord());
        }
        ss << ']';
        return ss.str();
    }

    auto TracksToBumpsNet::port_number() const -> std::usize {
        return this->_begin_tracks.size() + this->_end_bumps.size();
    }
}
