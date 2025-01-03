#include <catch2/catch_test_macros.hpp>
#include <std/string.hh>
#include <std/file.hh>
#include <debug/debug.hh>
#include <parse/reader/module.hh>
#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>


#define PLEASE_DO_NOT_FAIL(id, info) \
    WHEN("Case " #id ": " info) {\
        std::FilePath config_path{"../test/regression_test/case" #id};\
        auto [interposer, basedie] = kiwi::parse::read_config(config_path);\
        auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});\
        THEN("The total length should be within a limit"){\
            std::ifstream golden_file(config_path / "golden.txt");\
            if (!golden_file.is_open()){\
                debug::exception_in("regression test case " #id, "golden file open failure");\
            }\
            std::String golden_length;\
            if (!(golden_file >> golden_length)) { \
                debug::debug("golden file is empty in regression test case " #id);\
            }\
            else {\
                CHECK(total_length <= std::stoi(golden_length));\
            }\
        }\
    }

namespace kiwi::test{

    // SCENARIO("Regression test for basic kiwi functions", "[basic]"){
        
    //     GIVEN("Configs, describing connections, external_ports, topdies and topdie_insts"){
    //         //! notice: cob array here is 9*12
    //         PLEASE_DO_NOT_FAIL(1, "Muyan topdie with synchroinzed nets only");
    //         PLEASE_DO_NOT_FAIL(2, "Muyan topdie with both synchroinzed and unsynchronized nets");
    //         PLEASE_DO_NOT_FAIL(3, "Muyan topdie with unsynchronized nets only");
    //         // PLEASE_DO_NOT_FAIL(4, "A complete case with more nets and net types");
    //         // PLEASE_DO_NOT_FAIL(5, "test repeated connections in input file");
    //         PLEASE_DO_NOT_FAIL(6, "a case with more nets then case1");
            
    //     }
    // }

    SCENARIO("CPU-MEM-AI circuit test", "[CPU_MEM_AI]"){
        
        GIVEN("config.json & a txt file from xl"){
            //! notice: cob array here is 9*13, and available pose/nege port is different
            PLEASE_DO_NOT_FAIL(7, "a case with the least number of bus");
            PLEASE_DO_NOT_FAIL(8, "a case with a middle scale of bus");
            PLEASE_DO_NOT_FAIL(9, "a case with the most number of bus");
        }
    }

    SCENARIO("CPU-MEM circuit test", "[CPU_MEM]"){
        
        GIVEN("config.json & a txt file from xl"){
            //! notice: cob array here is 9*13, and available pose/nege port is the same with CPU_MEM_AI
            PLEASE_DO_NOT_FAIL(10, "a case with the least number of bus");
            PLEASE_DO_NOT_FAIL(11, "a case with a middle scale of bus");
            // PLEASE_DO_NOT_FAIL(12, "a case with the most number of bus");
        }
    }
}

