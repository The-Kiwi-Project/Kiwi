#pragma once

#include "./pin/pin.hh"
#include "./topdie/topdie.hh"
#include "./topdieinst/topdieinst.hh"
#include "circuit/export/export.hh"
#include "hardware/track/trackcoord.hh"

#include <std/utility.hh>
#include <std/memory.hh>
#include <std/integer.hh>
#include <std/collection.hh>
#include <std/string.hh>

namespace kiwi::circuit {

    class BaseDie {
    public:
        BaseDie() = default;

    public:
        auto add_topdie(std::String name, std::HashMap<std::String, std::usize> pin_map) -> TopDie*;
        auto add_topdie_inst(std::String name, TopDie* topdie, hardware::TOB* tob) -> TopDieInstance*;
        auto add_external_port(std::String name, const hardware::TrackCoord& coord) -> ExternalPort*;
        auto add_connections(int sync, Connection connection) -> void;
        auto add_net(std::Box<Net>) -> void;

    public:
        auto remove_topdie_inst(std::StringView name) -> bool;
        auto remove_external_port(std::StringView name) -> bool;

    public:
        void rename_topdie_inst(std::StringView old_name, std::String new_name);
        void rename_external_port(std::StringView old_name, std::String new_name);

    public:
        /*
            Get object form base die, if name not exit, return nullopt
        */
        auto get_topdie(std::StringView name) -> std::Option<TopDie*>;
        auto get_topdie_inst(std::StringView name) -> std::Option<TopDieInstance*>;
        auto get_external_port(std::StringView name) -> std::Option<ExternalPort*>;

    public:
        auto topdies() const -> const std::HashMap<std::StringView, std::Box<TopDie>>& { return this->_topdies; }
        auto connections() const -> const std::HashMap<int, std::Vector<Connection>> { return this->_connections; };        
        auto topdie_insts() const -> const std::HashMap<std::StringView, std::Box<TopDieInstance>>& { return this->_topdie_insts; }

        auto nets() -> std::Vector<std::Box<Net>>& { return this->_nets; }
        auto nets() const -> std::Span<const std::Box<Net>> { return this->_nets; }

    private:
        std::HashMap<std::StringView, std::Box<TopDie>> _topdies {};
        std::HashMap<std::StringView, std::Box<TopDieInstance>> _topdie_insts {};
        std::HashMap<std::StringView, std::Box<ExternalPort>> _external_ports {};
        std::HashMap<int, std::Vector<Connection>> _connections {};

        std::Vector<std::Box<Net>> _nets;
    };

}