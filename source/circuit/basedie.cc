#include "./basedie.hh"
#include "circuit/connection/connection.hh"
#include "circuit/connection/pin.hh"
#include "circuit/export/export.hh"
#include "circuit/topdie/topdie.hh"
#include "circuit/topdieinst/topdieinst.hh"
#include <hardware/tob/tob.hh>
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

    void BaseDie::clear() {
        this->_topdies.clear();
        this->_topdie_insts.clear();
        this->_external_ports.clear();
        this->_connections.clear();
        this->_pose_ports.clear();
        this->_nege_ports.clear();

        // MARK: ??????
        this->_nets.clear();
    }
    
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
        assert(topdie != nullptr);
        assert(tob != nullptr);

        debug::debug_fmt("Add topdie instance '{}' of '{}'", name, topdie->name());

        auto p = new TopDieInstance{std::move(name), topdie, tob};
        auto topdie_inst = std::Box<TopDieInstance>(p);

        auto res = this->_topdie_insts.emplace(topdie_inst->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("Topdie Instance '{}' already exit!", topdie_inst->name());
        }
        res.first->second = std::move(topdie_inst);
        tob->set_placed_instance(res.first->second.get());

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
        assert(res.first->first == sync);
        auto& iter = res.first->second.emplace_back(std::move(connection));
        return iter.get();
    }

    auto BaseDie::add_pose_port(const std::Vector<hardware::TrackCoord>& ports) -> void {
        this->_pose_ports = ports;
    }

    auto BaseDie::add_nege_port(const std::Vector<hardware::TrackCoord>& ports) -> void {
        this->_nege_ports = ports;
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

        // Remove all nets connection to this topdie instance 
        for (auto& [sync, connections] : this->_connections) {
            auto iter = std::remove_if(connections.begin(), connections.end(), [topdie_inst_ptr](std::Box<Connection>& c) -> bool {
                auto& input_pin = c->input_pin();
                auto is_input = input_pin.is_bump() && input_pin.to_connect_bump().inst == topdie_inst_ptr;

                auto& output_pin = c->output_pin();
                auto is_output = output_pin.is_bump() && output_pin.to_connect_bump().inst == topdie_inst_ptr;

                if (is_input || is_output) {
                    debug::debug_fmt("Remove net '{}'", *c);
                    return true;
                }
                return false;
            });

            connections.erase(iter, connections.end());
        } 

        // free tob 
        if (topdie_inst->tob() != nullptr) {
            assert(topdie_inst->tob()->placed_instance() == topdie_inst.get());
            topdie_inst->tob()->remove_placed_instance();
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
            for (auto i = connections.begin(); i != connections.end(); ++i) {
                debug::debug_fmt("B connection '{}'", *(i->get()));
            }

            auto iter = std::remove_if(connections.begin(), connections.end(), [port_ptr](const std::Box<Connection>& c) -> bool {
                auto& input_pin = c->input_pin();
                auto is_input = input_pin.is_external_port() && input_pin.to_connect_export().port == port_ptr;

                auto& output_pin = c->output_pin();
                auto is_output = output_pin.is_external_port() && output_pin.to_connect_export().port == port_ptr;
                
                if (is_input || is_output) {
                    debug::debug_fmt("Remove net '{}'", *c);
                    return true;
                }
                return false;
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
        assert(inst != nullptr);
        this->topdie_inst_rename(inst->name_view(), std::move(new_name));
    }

    void BaseDie::topdie_inst_rename(std::StringView old_name, std::String new_name) {
        debug::debug_fmt("Rename topdie instance '{}' to '{}'", old_name, new_name);

        if (old_name == new_name) {
            return;
        }

        if (this->_topdie_insts.contains(new_name)) {
            debug::exception_fmt("TopDie instance '{}' already exits!", new_name);
        }
        
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
        assert(eport != nullptr);
        this->external_port_rename(eport->name_view(), std::move(new_name));
    }

    void BaseDie::external_port_rename(std::StringView old_name, std::String new_name) {
        debug::debug_fmt("Rename external port '{}' to '{}'", old_name, new_name);
        
        if (old_name == new_name) {
            return;
        }

        if (this->_external_ports.contains(new_name)) {
            debug::exception_fmt("External port '{}' already exits!", new_name);
        }

        auto node = this->_external_ports.extract(old_name);
        if (!node) {
            debug::exception_fmt("External port '{}' not exit!", old_name);
        } else {
            auto eport = std::move(node.mapped());
            eport->set_name(std::move(new_name));
            this->_external_ports.emplace(eport->name_view(), std::move(eport));
        }
    }

    void BaseDie::external_port_set_coord(ExternalPort* eport, const hardware::TrackCoord& coord) {
        assert(eport != nullptr);
        debug::debug_fmt("Set coord for external port '{}' from '{}' to '{}'", eport->name_view(), eport->coord(), coord);

        // MARK check coord !
        auto coord_exit = false;
        auto eport_exit = false;
        for (auto& [_, ep] : this->_external_ports) {
            if (ep.get() == eport) {
                eport_exit = true;
                continue;
            }
            
            if (ep->coord() == coord) {
                coord_exit = true;
                break;
            }
        }

        if (coord_exit) {
            debug::debug_fmt("The coord '{}' already been used!", coord);
        }

        if (!eport_exit) {
            debug::debug_fmt("External port '{}' no exits", eport->name());
        }

        eport->set_coord(coord);
    }

    void BaseDie::external_port_set_coord(std::StringView old_name, const hardware::TrackCoord& coord) {
        auto res = this->_external_ports.find(old_name);
        if (res == this->_external_ports.end()) {
            debug::exception_fmt("External port '{}' not exit!", old_name);
        }

        this->external_port_set_coord(res->second.get(), coord);
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
