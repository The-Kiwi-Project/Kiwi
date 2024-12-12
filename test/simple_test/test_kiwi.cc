#include <std/string.hh>
#include <debug/debug.hh>

#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>

#include <parse/reader/module.hh>
#include <parse/writer/module.hh>
using namespace kiwi;

static auto test_muyan_xinzhai_no_bus() -> void 
try {
    auto [interposer, basedie] = kiwi::parse::read_config("../config/muyan_no_bus");
    algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
    interposer->randomly_map_remain_indexes();
    parse::write_control_bits(interposer.get(), "../test/simple_test/controlbit/muyan_no_bus.ctb");
}
THROW_UP_WITH("test_muyan_xinzhai_no_bus");

void test_kiwi_main() 
try {
    test_muyan_xinzhai_no_bus();
}
THROW_UP_WITH("test_kiwi_main")