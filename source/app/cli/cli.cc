#include "./cli.hh"
#include "algo/netbuilder/netbuilder.hh"

#include <hardware/interposer.hh>
#include <circuit/basedie.hh>

#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>

#include <parse/reader/module.hh>
#include <parse/writer/module.hh>

#include <std/utility.hh>
#include <std/range.hh>
#include <std/string.hh>
#include <debug/debug.hh>
#include <std/algorithm.hh>
#include <chrono>

namespace kiwi {

    auto cli_main(std::StringView config_path, std::Option<std::StringView> output_path) -> int {
        debug::initial_log("./debug.log");
        auto start_time = std::chrono::high_resolution_clock::now();
        auto [interposer, basedie] = kiwi::parse::read_config(config_path);

        algo::build_nets(basedie.get(), interposer.get());
        auto l = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;

        algo::show_route_result(basedie->nets(), l);
        debug::debug_fmt("Elapsed time: {} seconds", elapsed.count());

        interposer->randomly_map_remain_indexes();

        parse::write_control_bits(
            interposer.get(),
            output_path.has_value() ? *output_path : "./controlbit.ctb"    
        );

        return 0;
    }

}