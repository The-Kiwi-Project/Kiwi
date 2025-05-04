#include <hardware/interposer.hh>
#include <algo/placer/sa/saplacestrategy.hh>
#include <algo/placer/place.hh>
#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <algo/netbuilder/netbuilder.hh>
#include <parse/reader/module.hh>
#include <parse/writer/module.hh>
#include <global/debug/debug.hh>
#include <std/file.hh>
#include <chrono>


using namespace kiwi;

static std::Tuple<std::usize, std::usize, float> test_from_config(std::FilePath& config_path) {
    debug::debug_fmt("test file {}", config_path.string());

    try {
        debug::debug("Start parsing ...");
        auto start_time1 = std::chrono::high_resolution_clock::now();
        auto [interposer, basedie] = parse::read_config(config_path);
        
        if (!interposer || !basedie) {
            debug::error("Failed to load configuration file");
            return std::Tuple<std::usize, std::usize, float>{0, 0, 0};
        }
        algo::build_nets(basedie.get(), interposer.get());

        std::Vector<circuit::TopDieInstance*> topdies;
        for (auto& [name, topdie_inst] : basedie->topdie_insts()) {
            topdies.push_back(topdie_inst.get());
        }
        
        if (topdies.empty()) {
            debug::warning("No chip instances require layout");
            return std::Tuple<std::usize, std::usize, float>{0, 0, 0};
        }
        
        debug::debug("Start layout ...");
        auto strategy = algo::SAPlaceStrategy();
        place(interposer.get(), topdies, basedie.get(), strategy);
        assert(strategy.is_valid_placement(interposer.get(), topdies));
        auto end_time1 = std::chrono::high_resolution_clock::now();

        auto cost = evaluate_placement(interposer.get(), topdies, basedie.get(), strategy);
        debug::info_fmt("Layout evaluation result: {}", cost);
        debug::info("Layout result:");
        for (const auto& topdie : topdies) {
            if (topdie->tob()) {
                debug::info_fmt("Chip {} is located in {}", topdie->name(), topdie->tob()->coord());
            } else {
                debug::warning_fmt("Chip {} has not been assigned a TOB", topdie->name());
            }
        }

        debug::debug("Start routing ...");
        auto start_time2 = std::chrono::high_resolution_clock::now();
        auto l = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});
        auto end_time2 = std::chrono::high_resolution_clock::now();
        auto result = algo::show_route_result(basedie->nets(), l);

        std::chrono::duration<double> elapsed1 = end_time1 - start_time1;
        std::chrono::duration<double> elapsed2 = end_time2 - start_time2;
        debug::debug_fmt("Elapsed time: {} seconds", elapsed1.count() + elapsed2.count());

        interposer->randomly_map_remain_indexes();
        parse::write_control_bits(
            interposer.get(),
            "./controlbit.ctb"    
        );

        return result;
    }
    catch (std::exception& e) {
        debug::exception_fmt("unexpected exception: {}", e.what());
        return std::Tuple<std::usize, std::usize, float>{};
    }
}


void test_placeandroute_main() {
    std::FilePath config_path{"../test/config/case7"};

    float total_length{0}, max_length{0}, ave_length{0.0};
    for (auto i = 0; i < 10;) {
        auto [total, max, ave] = test_from_config(config_path);
        if (total != 0) {
            total_length += total;
            max_length += max;
            ave_length += ave;
            ++i;
        }
    }
    debug::info_fmt(
        "AVERAGE: Total Length: {}, Max Length: {}, SyncNet Average Length: {}",
        total_length / 10, max_length / 10, ave_length / 10
    );
    
}

