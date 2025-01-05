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
    
    class TopDieInstance {
    public:
        TopDieInstance(std::String name, TopDie* topdie, hardware::TOB* tob);

    public:
        auto move_to_tob(hardware::TOB* tob) -> void;
        auto swap_tob_with(TopDieInstance* other) -> void;
        auto add_net(Net* net) -> void;
        
    public:
        auto name() const -> const std::String& { return this->_name; }
        auto topdie() const -> TopDie* { return this->_topdie; }
        auto tob() const -> hardware::TOB* { return this->_tob; }
        auto nets() const -> const std::Vector<Net*> { return this->_nets; }

    private:
        std::String _name;
        TopDie* _topdie;
        hardware::TOB* _tob;
        std::Vector<Net*> _nets; 
        
    };

}