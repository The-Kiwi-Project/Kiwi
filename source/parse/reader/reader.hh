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

#include "./config/config.hh"
#include <circuit/connection/connection.hh>
#include <circuit/topdieinst/topdieinst.hh>
#include <hardware/track/trackcoord.hh>

#include <std/memory.hh>
#include <std/utility.hh>
#include <std/collection.hh>
#include <std/string.hh>

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
        auto add_external_ports_to_basedie() -> void;
        auto add_connections_to_basedie() -> void;
        auto add_01ports_to_basedie() -> void;

        auto parse_connection_pin(std::StringView name) -> circuit::Pin;
        auto parse_01(const std::HashMap<std::String, hardware::TrackCoord>& ports) -> std::Vector<hardware::TrackCoord>;
        
    private:
        const Config& _config;
        std::Box<hardware::Interposer> _interposer; 
        std::Box<circuit::BaseDie> _basedie;
    };

}