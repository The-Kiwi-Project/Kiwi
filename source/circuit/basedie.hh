#pragma once

#include "./connection/connection.hh"
#include "./topdie/topdie.hh"
#include "./topdieinst/topdieinst.hh"
#include "./export/export.hh"
#include <hardware/track/trackcoord.hh>

#include <std/utility.hh>
#include <std/memory.hh>
#include <std/integer.hh>
#include <std/collection.hh>
#include <std/string.hh>

namespace kiwi::circuit {

    class BaseDie {
    public:
        BaseDie() = default;
        ~BaseDie() noexcept;

    public:
        void clear();

    public:
        auto add_topdie(std::String name, std::HashMap<std::String, std::usize> pin_map) -> TopDie*;
        auto add_topdie_inst(std::String name, TopDie* topdie, hardware::TOB* tob) -> TopDieInstance*;
        auto add_topdie_inst(TopDie* topdie, hardware::TOB* tob) -> TopDieInstance*;
        auto add_external_port(std::String name, const hardware::TrackCoord& coord) -> ExternalPort*;
        auto add_external_port(const hardware::TrackCoord& coord) -> ExternalPort*;
        auto add_connection(int sync, Pin input, Pin output) -> Connection*;
        auto add_pose_port(const std::Vector<hardware::TrackCoord>& ports) -> void;
        auto add_nege_port(const std::Vector<hardware::TrackCoord>& ports) -> void;

        auto add_net(std::Box<Net>) -> void;

    public:
        auto remove_topdie_inst(std::StringView name) -> bool;
        auto remove_topdie_inst(TopDieInstance* inst) -> bool;

        auto remove_external_port(std::StringView name) -> bool;
        auto remove_external_port(ExternalPort* eport) -> bool;

        auto remove_connection(Connection*) -> bool;

    public:
        void topdie_inst_rename(TopDieInstance* inst, std::String new_name);
        void topdie_inst_rename(std::StringView old_name, std::String new_name);

        void external_port_rename(ExternalPort* eport, std::String new_name);
        void external_port_rename(std::StringView old_name, std::String new_name);
        void external_port_set_coord(ExternalPort* eport, const hardware::TrackCoord& coord);
        void external_port_set_coord(std::StringView old_name, const hardware::TrackCoord& coord);

        void connection_set_input(Connection* connection, Pin input);
        void connection_set_output(Connection* connection, Pin output);
        void connection_set_sync(Connection* connection, int sync);

    public:
        /*
            Get object form base die, if name not exit, return nullopt
        */
        auto get_topdie(std::StringView name) -> std::Option<TopDie*>;
        auto get_topdie_inst(std::StringView name) -> std::Option<TopDieInstance*>;
        auto get_external_port(std::StringView name) -> std::Option<ExternalPort*>;

    private:
        auto default_topdie_inst_name(TopDie* topdie) -> std::String;
        auto default_external_port_name() -> std::String;

    public:
        auto topdies() const -> const std::HashMap<std::StringView, std::Box<TopDie>>& 
        { return this->_topdies; }
        
        auto topdie_insts() const -> const std::HashMap<std::StringView, std::Box<TopDieInstance>>& 
        { return this->_topdie_insts; }
        
        auto external_ports() const -> const std::HashMap<std::StringView, std::Box<ExternalPort>>&
        { return this->_external_ports; }

        auto connections() const -> const std::HashMap<int, std::Vector<std::Box<Connection>>>& 
        { return this->_connections; }

        auto pose_ports() const -> const std::Vector<hardware::TrackCoord>&
        { return this-> _pose_ports; }

        auto nege_ports() const -> const std::Vector<hardware::TrackCoord>&
        { return this-> _nege_ports; }

        auto nets() -> std::Vector<std::Box<Net>>& 
        { return this->_nets; }

        auto nets() const -> std::Span<const std::Box<Net>> 
        { return this->_nets; }

    private:
        std::HashMap<std::StringView, std::Box<TopDie>> _topdies {};
        std::HashMap<std::StringView, std::Box<TopDieInstance>> _topdie_insts {};
        std::HashMap<std::StringView, std::Box<ExternalPort>> _external_ports {};
        std::HashMap<int, std::Vector<std::Box<Connection>>> _connections {};
        std::Vector<hardware::TrackCoord> _pose_ports {};
        std::Vector<hardware::TrackCoord> _nege_ports {};

        std::Vector<std::Box<Net>> _nets;
    };

}