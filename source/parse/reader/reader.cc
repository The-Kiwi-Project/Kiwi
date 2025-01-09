/*

    1. Load config

    2. Create interposer

    3. Create basedie

    4. Add topdie to basedie (_name, _pins_map)

    5. Add topdieinst to basedie (_name, _topdie, _tob, _nets is empty now)

    6. Create Net

        for each sync in config.connections

        - if sync == -1 => create net to basedie
        - else, all net has the same sync will see as one sync net
            so, create sync, and add each net into it

        > basedie owns nets, topdieinst borrows nets 

*/

#include "./reader.hh"

#include "./config/config.hh"
#include "circuit/connection/pin.hh"
#include <hardware/bump/bump.hh>
#include <hardware/track/track.hh>
#include <hardware/interposer.hh>
#include <hardware/tob/tob.hh>
#include <circuit/basedie.hh>
#include <circuit/topdie/topdie.hh>
#include <circuit/topdieinst/topdieinst.hh>
#include <circuit/net/nets.hh>

#include <debug/debug.hh>
#include <utility/string.hh>
#include <std/collection.hh>
#include <std/string.hh>
#include <std/utility.hh>
#include <std/range.hh>
#include <std/memory.hh>

namespace kiwi::parse {

    Reader::Reader(const Config& config) :
        _config{config},
        _interposer{},
        _basedie{}
    {
        this->_interposer = this->create_interposer();
        this->_basedie = this->create_basedir();
    }

    auto Reader::build() -> std::Tuple<std::Box<hardware::Interposer>, std::Box<circuit::BaseDie>>
    try {
        this->add_topdies_to_basedie();
        this->add_topdieinst_to_basedie();
        this->add_external_ports_to_basedie();
        this->add_connections_to_basedie();
        return { std::move(this->_interposer), std::move(this->_basedie) };
    }
    THROW_UP_WITH("Build system")

    auto Reader::create_interposer() -> std::Box<hardware::Interposer> {
        return std::make_unique<hardware::Interposer>();
    }

    auto Reader::create_basedir() -> std::Box<circuit::BaseDie> {
        return std::make_unique<circuit::BaseDie>();
    }

    auto Reader::add_topdies_to_basedie() -> void 
    try {
        auto& topdies_config = this->_config.topdies;

        for (auto& [name, config] : topdies_config) {
            this->_basedie->add_topdie(name, config.pin_map);
        }
    }
    THROW_UP_WITH("Add topdies to baesedie")

    auto Reader::add_topdieinst_to_basedie() -> void 
    try {
        auto& topdie_insts = this->_config.topdie_insts;

        for (auto& [inst_name, config] : topdie_insts) {
            auto& topdie_name = config.topdie;
            auto& coord = config.coord;

            // Get tob from interposer
            auto tob = this->_interposer->get_tob(coord);
            if (!tob.has_value()) {
                debug::exception_fmt("Invald coord '{}'", coord);
            }

            // Get topdie from baesdie
            auto res = this->_basedie->topdies().find(topdie_name);
            if (res == this->_basedie->topdies().end()) {
                debug::exception_fmt("No exit topdie '{}'", topdie_name);
            }
            auto& topdie = res->second;

            // Add into basedie
            this->_basedie->add_topdie_inst(inst_name, topdie.get(), *tob);
        }
    }
    THROW_UP_WITH("Add topdies inst to baesedie")

    auto Reader::add_external_ports_to_basedie() -> void 
    try {
        for (auto& [port_name, config] : this->_config.external_ports) {
            this->_basedie->add_external_port(port_name, config.coord);
        }
    }
    THROW_UP_WITH("Add external ports")

    auto Reader::add_connections_to_basedie() -> void
    try {
        for (auto& [sync, connections] : this->_config.connections) {
            for (const auto& connection : connections) {
                auto input = this->parse_connection_pin(connection.input);
                auto output = this->parse_connection_pin(connection.output);
                this->_basedie->add_connection(
                    sync,
                    std::move(input),
                    std::move(output)
                );
            }
        }
    }
    THROW_UP_WITH("Add connections")

    auto Reader::parse_connection_pin(std::StringView name) -> circuit::Pin {
        if (name.ends_with("pose")) {
            return circuit::Pin::connect_vdd();
        } 
        else if (name.ends_with("nege")) {
            return circuit::Pin::connect_gnd();
        }
        else if (std::StringView::npos == name.find('.')) {
            /// 
            /// External port: Search external ports in config, and get track object in interposer
            ///
            auto eport = this->_basedie->get_external_port(name);
            if (!eport.has_value()) {
                debug::exception_fmt("No exit external port '{}' in basedie", name);
            }
            return circuit::Pin::connect_export(*eport);
        } 
        else {
            /// 
            /// Topdie inst: Search inst by name, and get bump index, and get bump object in interposer
            ///
            auto tokens = utility::split(name, '.');
            if (tokens.size() != 2) {
                debug::exception_fmt("Invalid topinst' pin name '{}'", name);
            }
            auto topdie_inst_name = tokens[0];
            auto pin_name = tokens[1];

            // Is inst exit?
            auto inst = this->_basedie->get_topdie_inst(topdie_inst_name);
            if (!inst.has_value()) {
                debug::exception_fmt("No exit topdie inst '{}'", topdie_inst_name);
            }

            // Is pin exit?
            auto topdie = (*inst)->topdie();
            auto res = topdie->pins_map().find(std::String{pin_name});
            if (res == topdie->pins_map().end()) {
                debug::exception_fmt("No exit pin name '{}' in topdie '{}'", pin_name, topdie->name());
            }
            
            return circuit::Pin::connect_bump(*inst, std::String{pin_name});
        }
    }

}