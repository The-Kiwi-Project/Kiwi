#include "./basedie.hh"
#include "circuit/export/export.hh"
#include "circuit/topdie/topdie.hh"
#include "circuit/topdieinst/topdieinst.hh"
#include "std/string.hh"
#include <debug/debug.hh>
#include <memory>

namespace kiwi::circuit {
    
    auto BaseDie::add_topdie(std::String name, std::HashMap<std::String, std::usize> pin_map) -> TopDie* {
        auto topdie = std::make_unique<TopDie>(std::move(name), std::move(pin_map));
        auto res = this->_topdies.emplace(topdie->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("Topdie '{}' already exit!", topdie->name());
        }
        res.first->second = std::move(topdie);
        return res.first->second.get();
    }

    auto BaseDie::add_topdie_inst(std::String name, TopDie* topdie, hardware::TOB* tob) -> TopDieInstance* {
        auto topdie_inst = std::make_unique<TopDieInstance>(std::move(name), topdie, tob);
        auto res = this->_topdie_insts.emplace(topdie_inst->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("Topdie Instance '{}' already exit!", topdie_inst->name());
        }
        res.first->second = std::move(topdie_inst);
        return res.first->second.get();
    }

    auto BaseDie::add_external_port(std::String name, const hardware::TrackCoord& coord) -> ExternalPort* {
        auto external_port = std::make_unique<ExternalPort>(std::move(name), coord);
        auto res = this->_external_ports.emplace(external_port->name_view(), nullptr);
        if (res.second == false) {
            debug::exception_fmt("External port '{}' already exit!", external_port->name());
        }
        res.first->second = std::move(external_port);
        return res.first->second.get();
    }

    auto BaseDie::add_net(std::Box<Net> net) -> void {
        this->_nets.emplace_back(std::move(net));
    }

    auto BaseDie::add_connections(int sync, Connection connection) -> void {
        auto& cnt_vec = this->_connections.emplace(sync, std::Vector<Connection>{}).first->second;
        cnt_vec.emplace_back(std::move(connection));
    }

    auto BaseDie::remove_topdie_inst(std::StringView name) -> bool {
        return this->_topdie_insts.erase(name);
    }

    auto BaseDie::remove_external_port(std::StringView name) -> bool {
        return this->_external_ports.erase(name);
    }

    void BaseDie::rename_topdie_inst(std::StringView old_name, std::String new_name) {
        auto node = this->_topdie_insts.extract(old_name);
        if (!node) {
            debug::exception_fmt("Topdie Instance '{}' not exit!", old_name);
        } else {
            auto inst = std::move(node.mapped());
            inst->set_name(std::move(new_name));
            this->_topdie_insts.emplace(inst->name_view(), std::move(inst));
        }
    }

    void BaseDie::rename_external_port(std::StringView old_name, std::String new_name) {
        auto node = this->_external_ports.extract(old_name);
        if (!node) {
            debug::exception_fmt("Topdie Instance '{}' not exit!", old_name);
        } else {
            auto eport = std::move(node.mapped());
            eport->set_name(std::move(new_name));
            this->_external_ports.emplace(eport->name_view(), std::move(eport));
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

}
