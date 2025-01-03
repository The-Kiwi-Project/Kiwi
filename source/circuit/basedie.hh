#pragma once

#include "./pin/pin.hh"
#include "./topdie/topdie.hh"
#include "./topdieinst/topdieinst.hh"

#include <std/utility.hh>
#include <std/memory.hh>
#include <std/integer.hh>
#include <std/collection.hh>
#include <std/string.hh>

namespace kiwi::circuit {

    class BaseDie {
    public:
        auto add_topdie(std::String name, std::HashMap<std::String, std::usize> pin_map) -> void;
        auto add_topdie_inst(std::String name, TopDie* topdie, hardware::TOB* tob) -> void;
        auto add_net(std::Box<Net>) -> void;
        auto add_connections(int sync, Connection connection) -> void;

        auto get_topdie(std::StringView name) -> std::Option<TopDie*>;
        auto get_topdie_inst(std::StringView name) -> std::Option<TopDieInstance*>;

    public:
        auto topdies() -> std::HashMap<std::String, std::Box<TopDie>>& { return this->_topdies; }
        auto topdies() const -> const std::HashMap<std::String, std::Box<TopDie>>& { return this->_topdies; }

        auto connections() -> std::HashMap<int, std::Vector<Connection>> { return this->_connections; };
        auto connections() const -> const std::HashMap<int, std::Vector<Connection>> { return this->_connections; };
        
        auto topdie_insts() -> std::HashMap<std::String, std::Box<TopDieInstance>>& { return this->_topdie_insts; }
        auto topdie_insts() const -> const std::HashMap<std::String, std::Box<TopDieInstance>>& { return this->_topdie_insts; }

        auto nets() -> std::Vector<std::Box<Net>>& { return this->_nets; }
        auto nets() const -> std::Span<const std::Box<Net>> { return this->_nets; }

    private:
        std::HashMap<std::String, std::Box<TopDie>> _topdies;
        std::HashMap<std::String, std::Box<TopDieInstance>> _topdie_insts;
        std::HashMap<int, std::Vector<Connection>> _connections;

        std::Vector<std::Box<Net>> _nets;
    };

}