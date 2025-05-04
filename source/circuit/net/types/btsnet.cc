#include "./btsnet.hh"
#include <hardware/bump/bump.hh>


namespace kiwi::circuit {

    BumpToTracksNet::BumpToTracksNet(hardware::Bump* begin_bump, std::Vector<hardware::Track*> end_tracks) :
        _begin_bump{begin_bump},
        _end_tracks{std::move(end_tracks)},
        Net{Priority{1}}
    {
    }

    BumpToTracksNet::~BumpToTracksNet() noexcept {

    }

    auto BumpToTracksNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        this->_begin_bump = hardware::Bump::update_bump(this->_begin_bump, prev_tob, next_tob);
    }

    auto BumpToTracksNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_bump_to_tracks_net(interposer, this);
    }

    auto BumpToTracksNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        this->_priority = Priority{this->_priority.value() + bias};
    }

    auto BumpToTracksNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto track : this->end_tracks()) {
            coords.emplace_back(track->coord());
        }
        coords.emplace_back(this->begin_bump()->coord());
        return coords;
    }

    auto BumpToTracksNet::check_accessable_cobunit() -> void {
        std::HashSet<std::usize> accessable_cobunit {};
        std::usize bank_size = hardware::COB::INDEX_SIZE/2;
        std::usize wilton_size = hardware::COBUnit::WILTON_SIZE;

        for (auto track: _end_tracks){
            std::usize index = track->coord().index;
            std::usize id {(index/bank_size)*wilton_size + (index%wilton_size)};
            accessable_cobunit.emplace(id);
        }
        _begin_bump->intersect_access_unit(accessable_cobunit);
    }

    auto BumpToTracksNet::to_string() const -> std::String {
        auto ss = std::StringStream {};
        ss << std::format("Begin bump '{}' to End tracks '[", this->_begin_bump->coord());
        for (int i = 0; i < this->_end_tracks.size(); ++i) {
            if (i != 0) {
                ss << ", ";
            }
            ss << std::format("{}", this->_end_tracks[i]->coord());
        }
        ss << ']';
        return ss.str();
    }

    auto BumpToTracksNet::port_number() const -> std::usize {
        return (this->_end_tracks.size() + 1);
    }

    auto BumpToTracksNet::has_tob_in_ports(hardware::TOB* tob) const -> bool {
        return this->_begin_bump->tob()->coord() == tob->coord();
    }
    
}
