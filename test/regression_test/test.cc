#include <catch2/catch_test_macros.hpp>
#include <std/string.hh>
#include <std/file.hh>
#include <debug/debug.hh>
#include <parse/reader/module.hh>
#include <algo/router/route.hh>
#include <algo/router/maze/mazeroutestrategy.hh>
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>


namespace kiwi::test {

    SCENARIO("Regression test for basic kiwi functions", "[basic]"){

        GIVEN("Configs, describing connections, external_ports, topdies and topdie_insts"){
            
            // WHEN("Case 1: Muyan topdie with synchroinzed nets only"){
            //     std::FilePath config_path{"../test/regression_test/case1"};
            //     auto [interposer, basedie] = kiwi::parse::read_config(config_path);
            //     auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});

            //     THEN("The total length should be within a limit"){
            //         std::ifstream golden_file(config_path / "golden.txt");
            //         if (!golden_file.is_open()){
            //             debug::exception_in("regression test case 1", "golden file open failure");
            //         }

            //         std::String golden_length;
            //         if (!(golden_file >> golden_length)) { 
            //             debug::exception_in("regression test case 1", "golden file read failure");
            //         }

            //         CHECK(total_length <= std::stoi(golden_length));
            //     }
            // }

            WHEN("Case 2: Muyan topdie with both synchroinzed and unsynchronized nets"){
                std::FilePath config_path{"../test/regression_test/case2"};
                auto [interposer, basedie] = kiwi::parse::read_config(config_path);
                auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});

                THEN("The total length should be within a limit"){
                    std::ifstream golden_file(config_path / "golden.txt");
                    if (!golden_file.is_open()){
                        debug::exception_in("regression test case 2", "golden file open failure");
                    }

                    std::String golden_length;
                    if (!(golden_file >> golden_length)) { 
                        debug::exception_in("regression test case 2", "golden file read failure");
                    }

                    CHECK(total_length <= std::stoi(golden_length));
                }
            }

            // WHEN("Case 3: Muyan topdie with unsynchronized nets only"){
            //     std::FilePath config_path{"../test/regression_test/case3"};
            //     auto [interposer, basedie] = kiwi::parse::read_config(config_path);
            //     auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});

            //     THEN("The total length should be within a limit"){
            //         std::ifstream golden_file(config_path / "golden.txt");
            //         if (!golden_file.is_open()){
            //             debug::exception_in("regression test case 3", "golden file open failure");
            //         }

            //         std::String golden_length;
            //         if (!(golden_file >> golden_length)) { 
            //             debug::exception_in("regression test case 3", "golden file read failure");
            //         }

            //         CHECK(total_length <= std::stoi(golden_length));
            //     }
            // }
    
            // WHEN("Case 4: A complete case with more nets & IO, including positive/negative ports"){
            //     std::FilePath config_path{"../test/regression_test/case4"};
            //     auto [interposer, basedie] = kiwi::parse::read_config(config_path);
            //     auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});

            //     THEN("The total length should be within a limit"){
            //         std::ifstream golden_file(config_path / "golden.txt");
            //         if (!golden_file.is_open()){
            //             debug::exception_in("regression test case 4", "golden file open failure");
            //         }

            //         std::String golden_length;
            //         if (!(golden_file >> golden_length)) { 
            //             debug::exception_in("regression test case 4", "golden file read failure");
            //         }

            //         CHECK(total_length <= std::stoi(golden_length));
            //     }
            // }

            // WHEN("Case 5: Test repeated connections"){
            //     std::FilePath config_path{"../test/regression_test/case5"};
            //     auto [interposer, basedie] = kiwi::parse::read_config(config_path);
            //     auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});

            //     THEN("The total length should be within a limit"){
            //         std::ifstream golden_file(config_path / "golden.txt");
            //         if (!golden_file.is_open()){
            //             debug::exception_in("regression test case 5", "golden file open failure");
            //         }

            //         std::String golden_length;
            //         if (!(golden_file >> golden_length)) { 
            //             debug::exception_in("regression test case 5", "golden file read failure");
            //         }

            //         CHECK(total_length <= std::stoi(golden_length));
            //     }
            // }

            // WHEN("Case 6: A case with a bit more connections then case1"){
            //     std::FilePath config_path{"../test/regression_test/case6"};
            //     auto [interposer, basedie] = kiwi::parse::read_config(config_path);
            //     auto total_length = algo::route_nets(interposer.get(), basedie.get(), algo::MazeRouteStrategy{});

            //     THEN("The total length should be within a limit"){
            //         std::ifstream golden_file(config_path / "golden.txt");
            //         if (!golden_file.is_open()){
            //             debug::exception_in("regression test case 6", "golden file open failure");
            //         }

            //         std::String golden_length;
            //         if (!(golden_file >> golden_length)) { 
            //             debug::exception_in("regression test case 6", "golden file read failure");
            //         }

            //         CHECK(total_length <= std::stoi(golden_length));
            //     }
            // }
        }
    }

}

