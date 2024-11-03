#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <algo/placer/sa/saplacestrategy.hh>

using namespace kiwi::algo;
using namespace kiwi::hardware;
using namespace kiwi::circuit;

static void test_sa_place_strategy() {
    auto i = Interposer{};
    auto strategy = SAPlaceStrategy{};


}

void test_placer_main() {
    test_sa_place_strategy();
}