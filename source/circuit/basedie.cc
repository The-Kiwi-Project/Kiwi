#include "./basedie.hh"
#include "circuit/topdie/topdie.hh"
#include "circuit/topdieinst/topdieinst.hh"
#include "std/string.hh"
#include <memory>

namespace kiwi::circuit {
    
    auto BaseDie::add_topdie(std::String name, std::HashMap<std::String, std::usize> pin_map) -> void {
        auto topdie = std::make_unique<TopDie>(name, std::move(pin_map));
        this->_topdies.emplace(std::move(name), std::move(topdie));
    }

    auto BaseDie::add_topdie_inst(std::String name, TopDie* topdie, hardware::TOB* tob) -> void {
        auto topdie_inst = std::make_unique<TopDieInstance>(name, topdie, tob);
        this->_topdie_insts.emplace(std::move(name), std::move(topdie_inst));
    }

    auto BaseDie::add_net(std::Box<Net> net) -> void {
        this->_nets.emplace_back(std::move(net));
    }

    auto BaseDie::add_connections(int sync, Connection connection) -> void {
        auto& cnt_vec = this->_connections.emplace(sync, std::Vector<Connection>{}).first->second;
        cnt_vec.emplace_back(std::move(connection));
    }

    auto BaseDie::get_topdie(std::StringView name) -> std::Option<TopDie*> {
        auto res = this->_topdies.find(std::String{name});
        if (res == this->_topdies.end()) {
            return std::nullopt;
        } else {
            return {res->second.get()};
        }
    }

    auto BaseDie::get_topdie_inst(std::StringView name) -> std::Option<TopDieInstance*> {
        auto res = this->_topdie_insts.find(std::String{name});
        if (res == this->_topdie_insts.end()) {
            return std::nullopt;
        } else {
            return {res->second.get()};
        }
    }

}
