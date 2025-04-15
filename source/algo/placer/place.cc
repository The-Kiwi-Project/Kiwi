#include "./place.hh"
#include "./placestrategy.hh"
#include "debug/debug.hh"
#include <circuit/basedie.hh>

namespace kiwi::algo {

    auto place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies,
        const PlaceStrategy& strategy
    )-> void {
        place(interposer, topdies, nullptr, strategy);
    }

    auto place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies,
        circuit::BaseDie* basedie,
        const PlaceStrategy& strategy
    )-> void {
        debug::info("Start Layout");
        
        if (!interposer) {
            debug::error("Interposer pointer is empty");
            return;
        }
        
        if (topdies.empty()) {
            debug::warning("No top-level chip instance needs to be laid out.");
            return;
        }
        
        if (!basedie) {
            debug::warning("BaseDie pointer is empty, limited functionality");
        }
        
        strategy.place(interposer, topdies);

        auto cost = evaluate_placement(interposer, topdies, basedie, strategy);
        debug::info_fmt("Layout completed, total cost: {}", cost);
    }
    
    auto evaluate_placement(
        hardware::Interposer* interposer,
        const std::Vector<circuit::TopDieInstance>& topdies,
        circuit::BaseDie* basedie,
        const PlaceStrategy& strategy
    ) -> std::i64 {
        return strategy.evaluate_placement(interposer, topdies, basedie);
    }

}