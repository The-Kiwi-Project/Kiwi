#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <circuit/topdieinst/topdieinst.hh>
#include <circuit/net/net.hh>
#include <circuit/topdie/topdie.hh>

#include <algo/placer/sa/saplacestrategy.hh>
#include <algo/placer/place.hh>
#include <algo/netbuilder/netbuilder.hh>
#include <parse/reader/module.hh>

#include <debug/debug.hh>
#include "./utilty.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

using namespace kiwi::algo;
using namespace kiwi::hardware;
using namespace kiwi::circuit;
using namespace kiwi::parse;
using namespace kiwi;

static void test_basic_placement() {
    debug::debug("Test layout function"); 
    auto interposer = Interposer{};

    std::Vector<std::shared_ptr<TopDieInstance>> topdie_ptrs;
    auto topdie1 = std::make_shared<TopDieInstance>("topdie1", nullptr);
    auto topdie2 = std::make_shared<TopDieInstance>("topdie2", nullptr);
    auto topdie3 = std::make_shared<TopDieInstance>("topdie3", nullptr);
    
    topdie_ptrs.push_back(topdie1);
    topdie_ptrs.push_back(topdie2);
    topdie_ptrs.push_back(topdie3);
    
    std::Vector<TopDieInstance> topdies;
    for (const auto& ptr : topdie_ptrs) {
        topdies.push_back(*ptr);
    }
    
    // Simulated Annealing Layout
    auto strategy = SAPlaceStrategy{
        100.0,  // initial temperature
        0.5,    // freezing temperature
        50,     // solution times
        0.95,   // cooling rate
        50,     // maximum times
        1.0,    // line length
        0.5,    // jam
        0.3,    // heat distribution
        0.2     // power consumption
    };
    place(&interposer, topdies, strategy);
    ASSERT(strategy.is_valid_placement(&interposer, topdies));

    debug::info("Layout result:");
    for (const auto& topdie : topdies) {
        if (topdie.tob()) {
            debug::info_fmt("Chip {} is located in {}", topdie.name(), topdie.tob()->coord().to_string());
        } else {
            debug::warning_fmt("Chip {} has not been assigned a TOB", topdie.name());
        }
    }
}

// test case
static void test_placement_from_config(const std::String& case_path) {
    debug::debug_fmt("Configuration file test layout: {}", case_path);
    try {
        auto [interposer, basedie] = read_config(case_path);
        
        if (!interposer || !basedie) {
            debug::error("Failed to load configuration file");
            return;
        }
        build_nets(basedie.get(), interposer.get());

        std::Vector<TopDieInstance> topdies;
        for (auto& [name, topdie_inst] : basedie->topdie_insts()) {
            topdies.push_back(*topdie_inst);
        }
        
        if (topdies.empty()) {
            debug::warning("No chip instances require layout");
            return;
        }
        
        auto strategy = SAPlaceStrategy{};
        auto initial_placement = strategy.save_current_placement(topdies);
        place(interposer.get(), topdies, basedie.get(), strategy);
        ASSERT(strategy.is_valid_placement(interposer.get(), topdies));

        auto cost = evaluate_placement(interposer.get(), topdies, basedie.get(), strategy);
        debug::info_fmt("Layout evaluation result: {}", cost);
        debug::info("Layout result:");
        for (const auto& topdie : topdies) {
            if (topdie.tob()) {
                debug::info_fmt("Chip {} is located in {}", topdie.name(), topdie.tob()->coord().to_string());
            } else {
                debug::warning_fmt("Chip {} has not been assigned a TOB", topdie.name());
            }
        }
        strategy.restore_placement(topdies, initial_placement);    
    } catch (const std::exception& e) {
        debug::error_fmt("Test exception: {}", e.what());
    }
}

static void test_sa_place_strategy_components() {
    debug::debug("Layout testing components");
    auto interposer = Interposer{};
    auto strategy = SAPlaceStrategy{};
    
    double init_temp = 100.0;
    double temp = init_temp;
    for (std::size_t i = 0; i < 10; ++i) {
        temp = strategy.calculate_next_temperature(temp, i);
        debug::info_fmt("Iteration {}: Temperature={:.2f}", i, temp);
    }

    for (double t = init_temp; t > 0.5; t *= 0.8) {
        bool decision = strategy.decide_to_swap_topdie_inst(t);
        debug::info_fmt("Temperature {:.2f}: Exchange decision={}", t, decision ? "Yes" : "No");
    }
}

// res
static void test_placement_from_res_file(const std::String& res_file_path) {
    debug::debug_fmt("Res file test layout: {}", res_file_path);  
    try {
        auto interposer = std::make_shared<Interposer>();
        interposer->resize(20, 20);
        auto basedie = std::make_shared<BaseDie>();
        
        debug::info_fmt("Load res file: {}", res_file_path);
        std::ifstream file(res_file_path);
        if (!file.is_open()) {
            debug::error_fmt("Unable to open file: {}", res_file_path);
            return;
        }
        
        std::Vector<std::shared_ptr<TopDieInstance>> topdie_ptrs;
        std::HashMap<int, std::shared_ptr<TopDieInstance>> id_to_topdie;
        std::Vector<std::shared_ptr<Net>> net_ptrs;  
        std::String line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;       
            std::istringstream iss(line);
            int id, x1, y1, x2, y2, net_id;
            if (!(iss >> id >> x1 >> y1 >> x2 >> y2 >> net_id)) {
                iss.clear();
                iss.str(line);
                char dummy;
                if (!(iss >> id >> x1 >> y1 >> x2 >> y2 >> dummy >> net_id)) {
                    debug::warning_fmt("Unable to parse line: {}", line);
                    continue;
                }
            }

            if (!id_to_topdie.contains(id)) {
                auto topdie_name = "topdie_" + std::to_string(id);
                auto topdie = std::make_shared<TopDie>(topdie_name);
                auto topdie_inst = std::make_shared<TopDieInstance>(topdie_name + "_inst", topdie.get());
                basedie->add_topdie(topdie_name, topdie);
                basedie->add_topdie_inst(topdie_name + "_inst", topdie_inst);               
                id_to_topdie[id] = topdie_inst;
                topdie_ptrs.push_back(topdie_inst);
            }

            if (net_id > 0) {
                auto net_name = "net_" + std::to_string(net_id);
                auto net = std::make_shared<Net>(net_name);
                net_ptrs.push_back(net);
                id_to_topdie[id]->add_net(net.get());
            }
        }
        
        if (topdie_ptrs.empty()) {
            debug::warning("Failed to load");
            return;
        }       
        debug::info_fmt("Load {} chip instances", topdie_ptrs.size());

        std::Vector<TopDieInstance> topdies;
        for (const auto& ptr : topdie_ptrs) {
            topdies.push_back(*ptr);
        }
        
        auto strategy = SAPlaceStrategy{
            100.0,  
            0.5,    
            50,     
            0.95,   
            50,     
            1.0,    
            0.5,    
            0.3,    
            0.2     
        }; 
        place(interposer.get(), topdies, basedie.get(), strategy);
        ASSERT(strategy.is_valid_placement(interposer.get(), topdies));
        auto cost = evaluate_placement(interposer.get(), topdies, basedie.get(), strategy);
        debug::info_fmt("Layout evaluation result: Total cost= {}", cost);
        debug::info("Layout results:");
        for (const auto& topdie : topdies) {
            if (topdie.tob()) {
                debug::info_fmt("Chip {} is located in {}", topdie.name(), topdie.tob()->coord().to_string());
            } else {
                debug::warning_fmt("Chip {} has not been assigned a TOB", topdie.name());
            }
        }       
    } catch (const std::exception& e) {
        debug::error_fmt("Exception occurred: {}", e.what());
    }
}

// All test files
static void test_all_res_files() {
    debug::info("All test files");    
    const std::String res_dir = "../test/res";    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(res_dir)) {
            if (entry.is_regular_file()) {
                auto file_path = entry.path().string();
                if (file_path.ends_with(".txt")) {
                    debug::info_fmt("Test files: {}", file_path);
                    test_placement_from_res_file(file_path);
                }
            }
        }
    } catch (const std::exception& e) {
        debug::error_fmt("Exception occurred: {}", e.what());
    }
}

// main test
static void test_sa_place_strategy() {
    debug::info("Start testing");   
    test_basic_placement();
    test_sa_place_strategy_components();

    // case1-case6 test
    test_placement_from_config("../test/config/case1");
    test_placement_from_config("../test/config/case2");
    test_placement_from_config("../test/config/case3");
    test_placement_from_config("../test/config/case4");
    test_placement_from_config("../test/config/case5");
    test_placement_from_config("../test/config/case6");
    test_all_res_files();
    debug::info("Tests complete");
}
void test_placer_main() {
    test_sa_place_strategy();
}