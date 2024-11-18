#include "./place.hh"
#include "./placestrategy.hh"

namespace kiwi::algo {

    auto place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies,
        const PlaceStrategy& strategy
    )-> void {
        strategy.place(interposer, topdies);
    }

}