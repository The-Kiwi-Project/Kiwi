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
        _ttbnets{std::move(ttbnets)},
        Net{Priority{0}}
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

    auto SyncNet::update_priority(float bias) -> void {
        assert(0 <= bias && bias < 1);
        for (auto& net : this->_btbnets) {
            net->update_priority(bias);
        }

        for (auto& net : this->_bttnets) {
            net->update_priority(bias);
        }

        for (auto& net : this->_ttbnets) {
            net->update_priority(bias);
        }
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

    auto SyncNet::check_accessable_cobunit() -> void {
        for (auto& net: _btbnets) {
            net->check_accessable_cobunit();
        }
        for (auto& net: _bttnets) {
            net->check_accessable_cobunit();
        }
        for (auto& net: _ttbnets) {
            net->check_accessable_cobunit();
        }
    }

    
    auto SyncNet::to_string() const -> std::String {
        auto ss = std::StringStream {};
        ss << "Syncnet net:\n";
        for (auto& net: _btbnets) {
            ss << "    " << net->to_string() << '\n';
        }
        for (auto& net: _bttnets) {
            ss << "    " << net->to_string() << '\n';
        }
        for (auto& net: _ttbnets) {
            ss << "    " << net->to_string() << '\n';
        }
        ss << "End syncnet net";
        return ss.str();
    }

    auto SyncNet::port_number() const -> std::usize {
        std::usize port_number = 0;
        for (auto& net: _btbnets) {
            port_number += net->port_number();
        }
        for (auto& net: _bttnets) {
            port_number += net->port_number();
        }
        for (auto& net: _ttbnets) {
            port_number += net->port_number();
        }
        return port_number;
    }
}


