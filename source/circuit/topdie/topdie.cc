#include "./topdie.hh"


namespace kiwi::circuit{

    TopDie::TopDie(){};

    TopDie::TopDie(std::String name, std::HashMap<std::String, std::usize> pins_map) :
        _name{std::move(name)}, 
        _pins_map{std::move(pins_map)}
    {
        
    }
}


