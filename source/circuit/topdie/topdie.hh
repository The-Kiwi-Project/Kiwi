#pragma once

#include "std/utility.hh"
#include <std/integer.hh>
#include <std/collection.hh>
#include <std/string.hh>
#include <parse/reader/config/config.hh>

namespace kiwi::circuit {

    class TopDie {
    public:
        TopDie(std::String name, std::HashMap<std::String, std::usize> pins_map);
    
    public:
        auto get_bump_index(const std::String& pin_name) -> std::Option<std::usize>;

    public:
        auto name() const -> const std::String& { return this->_name; }
        auto name_view() const -> std::StringView { return this->_name; }
        auto pins_map() const -> const std::HashMap<std::String, std::usize>& { return this->_pins_map; }

    private:
        const std::String _name;
        const std::HashMap<std::String, std::usize> _pins_map;
    };

}