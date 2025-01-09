#include "./basedie.hh"
#include "circuit/connection/connection.hh"
#include "circuit/connection/pin.hh"
#include "circuit/export/export.hh"
#include "circuit/topdie/topdie.hh"
#include "circuit/topdieinst/topdieinst.hh"
#include "std/collection.hh"
#include "std/memory.hh"
#include "std/string.hh"
#include <algorithm>
#include <cassert>
#include <debug/debug.hh>
#include <format>
#include <memory>
#include <utility>

namespace kiwi::circuit {
    
    auto BaseDie::add_topdie(std::String name, std::HashMap<std::String, std::usize> pin_map) -> TopDie* {
        debug::debug_fmt("Add topdie '{}'", name);

        auto topdie = std::make_unique<TopDie>(std::move(name), std::move(pin_map));
        auto res = this->_topdies.emplace(topdie->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("Topdie '{}' already exit!", topdie->name());
        }
        res.first->second = std::move(topdie);
        return res.first->second.get();
    }

    auto BaseDie::add_topdie_inst(TopDie* topdie, hardware::TOB* tob) -> TopDieInstance* {
        auto name = this->default_topdie_inst_name(topdie);
        return this->add_topdie_inst(std::move(name), topdie, tob);
    }

    auto BaseDie::add_topdie_inst(std::String name, TopDie* topdie, hardware::TOB* tob) -> TopDieInstance* {
        debug::debug_fmt("Add topdie instance '{}' of '{}'", name, topdie->name());

        auto p = new TopDieInstance{std::move(name), topdie, tob};
        auto topdie_inst = std::Box<TopDieInstance>(p);

        auto res = this->_topdie_insts.emplace(topdie_inst->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("Topdie Instance '{}' already exit!", topdie_inst->name());
        }
        res.first->second = std::move(topdie_inst);
        return res.first->second.get();
    }

    auto BaseDie::add_external_port(const hardware::TrackCoord& coord) -> ExternalPort* {
        auto name = this->default_external_port_name();
        return this->add_external_port(std::move(name), coord);
    }

    auto BaseDie::add_external_port(std::String name, const hardware::TrackCoord& coord) -> ExternalPort* {
        debug::debug_fmt("Add external port '{}' in '{}'", name, coord);

        auto p = new ExternalPort{std::move(name), coord};
        auto external_port = std::Box<ExternalPort>{p};

        auto res = this->_external_ports.emplace(external_port->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("External port '{}' already exit!", external_port->name());
        }
        res.first->second = std::move(external_port);
        return res.first->second.get();
    }

    auto BaseDie::add_connection(int sync, Pin input, Pin output) -> Connection* {
        debug::debug_fmt("Add connection from '{}' to '{}'", input, output);

        auto p = new Connection{sync, std::move(input), std::move(output)};
        auto connection = std::Box<Connection>{p};
        
        auto res = this->_connections.emplace(sync, std::Vector<std::Box<Connection>>{});
        assert(res.first->first = sync);
        auto& iter = res.first->second.emplace_back(std::move(connection));
        return iter.get();
    }

    auto BaseDie::add_net(std::Box<Net> net) -> void {
        this->_nets.emplace_back(std::move(net));
    }

    auto BaseDie::remove_topdie_inst(TopDieInstance* inst) -> bool {
        if (inst == nullptr) {
            return false;
        }
        return this->remove_topdie_inst(inst->name_view());
    }

    auto BaseDie::remove_topdie_inst(std::StringView name) -> bool {
        debug::debug_fmt("Remove topdie instance '{}'", name);

        // return this->_topdie_insts.erase(name);
        auto node = this->_topdie_insts.extract(name);
        if (node.empty()) {
            return false;
        }

        auto topdie_inst = std::move(node.mapped());
        auto topdie_inst_ptr = topdie_inst.get();

        // Remove all topdie 
        for (auto& [sync, connections] : this->_connections) {
            auto iter = std::remove_if(connections.begin(), connections.end(), [topdie_inst_ptr](std::Box<Connection>& c) -> bool {
                auto is_input = std::match(c->input().connected_point(),
                    [](const ConnectExPort& _) -> bool {
                        return false;
                    },
                    [topdie_inst_ptr](const ConnectBump& bump) -> bool {
                        return bump.inst == topdie_inst_ptr;
                    }
                );

                auto is_output = std::match(c->output().connected_point(),
                    [](const ConnectExPort& _) -> bool {
                        return false;
                    },
                    [topdie_inst_ptr](const ConnectBump& bump) -> bool {
                        return bump.inst == topdie_inst_ptr;
                    }
                );

                return is_input || is_output;
            });

            connections.erase(iter, connections.end());
        } 

        return true;
    }

    auto BaseDie::remove_external_port(ExternalPort* eport) -> bool {
        if (eport == nullptr) {
            return false;
        }
        return this->remove_external_port(eport->name_view());
    }

    auto BaseDie::remove_external_port(std::StringView name) -> bool {
        debug::debug_fmt("Remove external port '{}'", name);

        auto node = this->_external_ports.extract(name);
        if (node.empty()) {
            return false;
        }

        auto port = std::move(node.mapped());
        auto port_ptr = port.get();
        
        // Remove all connection in with this external_port!
        for (auto& [sync, connections] : this->_connections) {
            auto iter = std::remove_if(connections.begin(), connections.end(), [port_ptr](std::Box<Connection>& c) -> bool {
                auto is_input = std::match(c->input().connected_point(),
                    [port_ptr](const ConnectExPort& cep) -> bool {
                        return cep.port == port_ptr;
                    },
                    [](const ConnectBump& cep) -> bool {
                        return false;
                    }
                );

                auto is_output = std::match(c->output().connected_point(),
                    [port_ptr](const ConnectExPort& cep) -> bool {
                        return cep.port == port_ptr;
                    },
                    [](const ConnectBump& cep) -> bool {
                        return false;
                    }
                );

                return is_input || is_output;
            });

            connections.erase(iter, connections.end());
        }

        return true;
    }

    auto BaseDie::remove_connection(Connection* connection) -> bool {
        if (connection == nullptr) {
            return false;
        }

        debug::debug_fmt("Remove connection '{}'", *connection);

        auto res = this->_connections.find(connection->sync());
        if (res == this->_connections.end()) {
            return false;
        }

        auto& connections = res->second;
        auto iter = std::remove_if(connections.begin(), connections.end(), [connection] (auto& c) {
            return c.get() == connection;
        });

        if (iter == connections.end()) {
            return false;
        }
        
        connections.erase(iter, connections.end());
        return true;
    }

    void BaseDie::topdie_inst_rename(TopDieInstance* inst, std::String new_name) {
        debug::debug_fmt("Rename topdie instance '{}' to '{}'", inst->name_view(), new_name);

        auto old_name = inst->name();
        
        auto node = this->_topdie_insts.extract(old_name);
        if (!node) {
            debug::exception_fmt("Topdie Instance '{}' not exit!", old_name);
        } else {
            auto inst = std::move(node.mapped());
            inst->set_name(std::move(new_name));
            this->_topdie_insts.emplace(inst->name_view(), std::move(inst));
        }
    }

    void BaseDie::external_port_rename(ExternalPort* eport, std::String new_name) {
        debug::debug_fmt("Rename external port '{}' to '{}'", eport->name_view(), new_name);

        auto old_name = eport->name();

        auto node = this->_external_ports.extract(old_name);
        if (!node) {
            debug::exception_fmt("Topdie Instance '{}' not exit!", old_name);
        } else {
            auto eport = std::move(node.mapped());
            eport->set_name(std::move(new_name));
            this->_external_ports.emplace(eport->name_view(), std::move(eport));
        }
    }

    void BaseDie::external_port_set_coord(ExternalPort* eport, const hardware::TrackCoord& coord) {
        debug::debug_fmt("Set coord for external port '{}' from '{}' to '{}'", eport->name_view(), eport->coord(), coord);

        // MARK, the same coord?
        eport->set_coord(coord);
    }

    void BaseDie::connection_set_input(Connection* connection, Pin input) {
        debug::debug_fmt("Set connection '{}' input to '{}'", *connection, input);

        connection->set_input(std::move(input));
    }

    void BaseDie::connection_set_output(Connection* connection, Pin output) {
        debug::debug_fmt("Set connection '{}' output to '{}'", *connection, output);

        connection->set_output(std::move(output));
    }

    void BaseDie::connection_set_sync(Connection* connection, int sync) {
        if (connection->sync() == sync) {
            return;
        } 

        debug::debug_fmt("Set connection '{}' sync form '{}' to '{}'", *connection, connection->sync(), sync);
        
        auto& old_group = this->_connections.at(connection->sync());
        
        auto removed_connection = std::Box<Connection>{nullptr};
        auto iter = std::remove_if(old_group.begin(), old_group.end(), [&removed_connection, connection] (std::Box<Connection>& c) {
            if (c.get() == connection) {
                removed_connection = std::move(c);
                return true;
            }
            return false;
        });

        if (removed_connection.get() != nullptr) {
            assert(remove_connection.get == connection);
            auto& new_group = this->_connections.emplace(sync, std::Vector<std::Box<Connection>>{}).first->second;
            new_group.emplace_back(std::move(removed_connection));
            connection->set_sync(sync);
            old_group.erase(iter, old_group.end());
        }
        else {
            debug::exception("Connection no in this basedie!");
        }
    }

    auto BaseDie::get_topdie(std::StringView name) -> std::Option<TopDie*> {
        auto res = this->_topdies.find(name);
        if (res == this->_topdies.end()) {
            return std::nullopt;
        } else {
            return {res->second.get()};
        }
    }

    auto BaseDie::get_topdie_inst(std::StringView name) -> std::Option<TopDieInstance*> {
        auto res = this->_topdie_insts.find(name);
        if (res == this->_topdie_insts.end()) {
            return std::nullopt;
        } else {
            return {res->second.get()};
        }
    }

    auto BaseDie::get_external_port(std::StringView name) -> std::Option<ExternalPort*> {
        auto res = this->_external_ports.find(name);
        if (res == this->_external_ports.end()) {
            return std::nullopt;
        } else {
            return {res->second.get()};
        }
    }

    auto BaseDie::default_topdie_inst_name(TopDie* topdie) -> std::String {
        auto size = 0;
        for (const auto& [name, inst] : this->_topdie_insts) {
            if (topdie == inst->topdie()) {
                size += 1;
            }
        }

        auto name = std::format("{}_{}", topdie->name(), size);
        while (this->_topdie_insts.contains(name)) {
            name.push_back('_');
        }

        return name;
    }

    auto BaseDie::default_external_port_name() -> std::String {
        auto name = std::format("externl_port_{}", this->_external_ports.size());
        while (this->_external_ports.contains(name)) {
            name.push_back('_');
        }
        return name;
    }

    BaseDie::~BaseDie() noexcept = default;
}
