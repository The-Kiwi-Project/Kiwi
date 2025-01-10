
#pragma once

#include <hardware/track/trackcoord.hh>

#include <std/collection.hh>
#include <std/memory.hh>
#include <std/utility.hh>
#include <std/string.hh>

namespace kiwi::hardware {
    class Track;
    class Bump;
    class Interposer;
}

namespace kiwi::circuit {
    class BaseDie;
    class Net;
    class Connection;
    class TopDieInstance;
    class Pin;
}

namespace kiwi::algo {

    void build_nets(circuit::BaseDie* basedie, hardware::Interposer* interposer);

    class NetBuilder {
    public:
        NetBuilder(circuit::BaseDie* basedie, hardware::Interposer* interposer);

    public:
        void build();

    private:
        auto build_no_sync_nets(std::Span<const std::Box<circuit::Connection>> connections) -> void;
        auto build_sync_net(std::Span<const std::Box<circuit::Connection>> connections) -> void;
        auto build_fixed_nets() -> void;
        auto build_01_ports() -> void;
     
        using Node = std::Variant<hardware::Track*, hardware::Bump*>;
        auto pin_to_node(const circuit::Pin& pin) -> Node;
        

    private:
        circuit::BaseDie* _basedie {nullptr};
        hardware::Interposer* _interposer {nullptr};

        // Temp var while build!
        std::Vector<hardware::TrackCoord> _pose_tracks;
        std::Vector<hardware::TrackCoord> _nege_tracks;
        std::Vector<hardware::Bump*> _bumps_with_pose {};
        std::Vector<hardware::Bump*> _bumps_with_nege {};
        std::HashMap<hardware::Bump*, circuit::TopDieInstance*> _bump_to_topdie_inst {};
    };

}