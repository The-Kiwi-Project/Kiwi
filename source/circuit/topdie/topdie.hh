#pragma once

#include "std/utility.hh"
#include <std/integer.hh>
#include <std/collection.hh>
#include <std/string.hh>

namespace kiwi::circuit {

    class TopDie {
    public:
        TopDie();
        TopDie(std::String name, std::HashMap<std::String, std::usize> pins_map);
    
    public:
        auto get_bump_index(std::String pin_name) -> std::Option<std::usize>;

    public:
        auto name() const -> std::StringView { return this->_name; }
        auto pins_map() const -> const std::HashMap<std::String, std::usize>& { return this->_pins_map; }

    private:
        std::String _name;
        std::HashMap<std::String, std::usize> _pins_map;
    };

}