#pragma once

#include "../topdie/topdie.hh"
#include "../net/net.hh"

#include <std/integer.hh>
#include <std/collection.hh>
#include <std/string.hh>

namespace kiwi::hardware {
    class TOB;
}

namespace kiwi::circuit {
    
    class BaseDie;

    class TopDieInstance {
        friend BaseDie;

    private:
        TopDieInstance(std::String name, TopDie* topdie, hardware::TOB* tob);

    public:
        // MARK algo method ...
        auto move_to_tob(hardware::TOB* tob) -> void;
        auto swap_tob_with(TopDieInstance* other) -> void;
        auto add_net(Net* net) -> void;
        
    public:
        auto name() const -> const std::String& 
        { return this->_name; }
        
        auto name_view() const -> std::StringView 
        { return this->_name; }
        
        auto topdie() const -> TopDie* 
        { return this->_topdie; }
        
        auto tob() const -> hardware::TOB* 
        { return this->_tob.value(); }
        
        auto nets() const -> const std::Vector<Net*> 
        { return this->_nets; }

        void set_name(std::String name) 
        { this->_name = std::move(name); } 

    public:
        // MARK. Move algo method to algo
        void place_to_idle_tob(hardware::TOB* tob);
        void swap_tob_with_(TopDieInstance* other);

    private:
        std::String _name;
        TopDie* _topdie;
        std::Option<hardware::TOB*> _tob;
        std::Vector<Net*> _nets; 
        
    };

}