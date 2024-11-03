#pragma once

/*

    1. Load config

    2. Create interposer

    3. Create basedie

    4. Add topdie to basedie (_name, _pins_map)

    5. Add topdieinst to basedie (_name, _topdie, _tob, _nets is empty now)

    6. Add Net

        for each sync in config.connections

        - if sync == -1 => create net to basedie
        - else, all net has the same sync will see as one sync net
            so, create sync, and add each net into it

        > basedie owns nets, topdieinst borrows nets 

*/

#include <std/memory.hh>
#include <std/utility.hh>
#include <std/collection.hh>
#include <std/string.hh>

#include "./config/config.hh"
#include "circuit/topdie/topdieinst.hh"
#include "hardware/node/trackcoord.hh"
#include "parse/reader/config/connection.hh"

namespace kiwi::hardware {
    class Interposer;
    class TOB;
    class Track;
    class Bump;
}

namespace kiwi::circuit {
    class BaseDie;
    class TopDie;

    class TrackToBumpNet;
    class BumpToTrackNet;
    class BumpToBumpNet;
    class TracksToBumpsNet;
    class SyncNet;
}

namespace kiwi::parse {

    class Config;

    class Reader {
    public: 
        Reader(const Config& config);

    public:
        auto build() -> std::Tuple<std::Box<hardware::Interposer>, std::Box<circuit::BaseDie>>;

    private:
        auto create_interposer() -> std::Box<hardware::Interposer>;
        auto create_basedir() -> std::Box<circuit::BaseDie>;

    private:
        auto add_topdies_to_basedie() -> void;
        auto add_topdieinst_to_basedie() -> void;
        auto add_nets() -> void;

    private:
        auto build_no_sync_nets(std::Span<const ConnectionConfig> connections) -> void;
        auto build_sync_net(std::Span<const ConnectionConfig> connections) -> void;
        auto build_fixed_nets() -> void;
     
        using Node = std::Variant<hardware::Track*, hardware::Bump*>;
        auto parse_connection_pin(std::StringView name) -> Node;
        
        static auto is_pose_pin(std::StringView name) -> bool;
        static auto is_nege_pin(std::StringView name) -> bool;
        static auto is_fixed_pin(std::StringView name) -> bool;

        static std::Vector<hardware::TrackCoord> _pose_tracks;
        static std::Vector<hardware::TrackCoord> _nege_tracks;
        
    private:
        const Config& _config;
        std::Box<hardware::Interposer> _interposer; 
        std::Box<circuit::BaseDie> _basedie;

        // Temp var while build!
        std::Vector<hardware::Bump*> _bumps_with_pose {};
        std::Vector<hardware::Bump*> _bumps_with_nege {};
        std::HashMap<hardware::Bump*, circuit::TopDieInstance*> _bump_to_topdie_inst {};
    };

}