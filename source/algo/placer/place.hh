#pragma once

#include <std/memory.hh>
#include <std/collection.hh>

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class Net;
    class TopDieInstance;
    class BaseDie;
}

namespace kiwi::algo {

    struct PlaceStrategy;

    auto place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies,
        const PlaceStrategy& strategy
    ) -> void;
    
    // BaseDie
    auto place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies,
        circuit::BaseDie* basedie,

        const PlaceStrategy& strategy
    ) -> void;    
    auto evaluate_placement(
        hardware::Interposer* interposer,
        const std::Vector<circuit::TopDieInstance>& topdies,
        circuit::BaseDie* basedie,
        const PlaceStrategy& strategy
    ) -> std::i64;
}