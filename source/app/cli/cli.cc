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

namespace kiwi {

    auto cli_main(std::StringView config_path, std::Option<std::StringView> output_path) -> int {
        debug::initial_log("./debug.log");
        auto [interposer, basedie] = kiwi::parse::read_config(config_path);

        algo::build_nets(basedie.get(), interposer.get());
        auto l = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
        debug::info_fmt("Length: '{}'", l);

        interposer->randomly_map_remain_indexes();

        parse::write_control_bits(
            interposer.get(),
            output_path.has_value() ? *output_path : "./controlbit.ctb"    
        );

        return 0;
    }

}