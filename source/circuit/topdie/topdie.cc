#include "./topdie.hh"


namespace kiwi::circuit{

    TopDie::TopDie(std::String name, std::HashMap<std::String, std::usize> pins_map) :
        _name{std::move(name)}, 
        _pins_map{std::move(pins_map)}
    {
    }

    auto TopDie::get_bump_index(const std::String& pin_name) -> std::Option<std::usize> {
        auto res = this->_pins_map.find(pin_name);
        if (res == this->_pins_map.end()) {
            return std::nullopt;
        } else {
            return {res->second};
        }
    }

}


