#include "./syncnet.hh"

#include <hardware/coord.hh>



namespace kiwi::circuit
{

    SyncNet::SyncNet(
        std::Vector<std::Box<BumpToBumpNet>> btbnets,
        std::Vector<std::Box<BumpToTrackNet>> bttnets,
        std::Vector<std::Box<TrackToBumpNet>> ttbnets
    ) : 
        _btbnets{std::move(btbnets)},
        _bttnets{std::move(bttnets)},
        _ttbnets{std::move(ttbnets)}
    {
    }

    auto SyncNet::update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void {
        for (auto& net : this->_btbnets) {
            net->update_tob_postion(prev_tob, next_tob);
        }
        
        for (auto& net : this->_bttnets) {
            net->update_tob_postion(prev_tob, next_tob);
        }

        for (auto& net : this->_ttbnets) {
            net->update_tob_postion(prev_tob, next_tob);
        }
    }

    auto SyncNet::route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize {
        return strategy.route_sync_net(interposer, this);
    }

    auto SyncNet::priority() const -> Priority {
        return {0};
    }

    auto SyncNet::coords() const -> std::Vector<hardware::Coord> {
        auto coords = std::Vector<hardware::Coord>{};
        for (auto& net : this->_btbnets) {
            for (auto& coord : net->coords()) {
                coords.emplace_back(coord);
            }
        }

        for (auto& net : this->_bttnets) {
            for (auto& coord : net->coords()) {
                coords.emplace_back(coord);
            }
        }

        for (auto& net : this->_ttbnets) {
            for (auto& coord : net->coords()) {
                coords.emplace_back(coord);
            }
        }

        return coords;
    }
}


