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

    struct PlaceStrategy {
        virtual auto place(
            hardware::Interposer* interposer,
            std::Vector<circuit::TopDieInstance>& topdies
        ) const -> void = 0;
        
        virtual auto evaluate_placement(
            hardware::Interposer* interposer,
            const std::Vector<circuit::TopDieInstance>& topdies,
            circuit::BaseDie* basedie
        ) const -> std::i64 = 0;
    };
}