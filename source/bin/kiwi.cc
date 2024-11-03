#include <std/string.hh>
#include <debug/debug.hh>

#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>

#include <parse/reader/module.hh>
#include <parse/writer/module.hh>

using namespace kiwi;

// TODO: CLI interface
int main(int argc, char** argv)
try {
    if (argc < 3) {
        debug::info("Usage: kiwi <input folder path> <output path>");
        debug::fatal("Too less argumets for 'kiwi'");
    }

    auto input_path = std::String{argv[1]};
    auto output_path = std::String{argv[2]};

    auto [interposer, basedie] = kiwi::parse::read_config(input_path);

    algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
    
    interposer->randomly_map_remain_indexes();

    parse::write_control_bits(interposer.get(), output_path);
}
catch(const std::exception& e) {
    debug::error_fmt("Kiwi failed: {}", e.what());
}