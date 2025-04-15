#include "./saplacestrategy.hh"
#include "debug/debug.hh"

#include <algorithm>
#include <hardware/interposer.hh>
#include <hardware/tob/tob.hh>
#include <hardware/tob/tobcoord.hh>

#include <circuit/net/net.hh>
#include <circuit/topdie/topdie.hh>
#include <circuit/topdieinst/topdieinst.hh>

#include <utility/random.hh>

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/math.hh>

namespace kiwi::algo {

    auto SAPlaceStrategy::place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies
    ) const -> void {
        debug::info("Start executing simulated annealing layout algorithm");

        auto nets = std::HashSet<circuit::Net*>{};
        for (const auto& topdie_inst : topdies) {
            for (auto net : topdie_inst.nets()) {
                nets.emplace(net);
            }
        }

        double temperature {this->_init_temperature}; 
        auto total_cost = this->total_net_cost(nets);
        
        auto best_cost = total_cost;
        auto best_solution = this->save_current_placement(topdies);
        
        std::size_t no_improvement_count = 0;
        
        std::size_t iteration = 0;
        while (temperature > this->_freeze_temperature && no_improvement_count < this->_max_no_improvement) {
            bool improved = false;
            
            for (std::usize n = 0; n < this->_solve_number; ++n) {
                int new_total_cost = total_cost;

                if (this->decide_to_swap_topdie_inst()) {
                    auto [topdie_inst1, topdie_inst2] = this->randomly_choise_two_topdie_insts(topdies);
                    
                    std::HashSet<circuit::Net*> changed_nets;
                    for (auto net : topdie_inst1->nets()) {
                        changed_nets.emplace(net);
                    }

                    for (auto net : topdie_inst2->nets()) {
                        changed_nets.emplace(net);
                    }

                    for (auto net : changed_nets) {
                        new_total_cost -= this->net_cost(net);
                    }

                    topdie_inst1->swap_tob_with(topdie_inst2);

                    for (auto net : changed_nets) {
                        new_total_cost += this->net_cost(net);
                    }

                    auto deltaCost = new_total_cost - total_cost;
                    
                    if (deltaCost <= 0) {
                        total_cost = new_total_cost;
                        improved = true;
                    } else {
                        if (random() <= std::exp(-1.0 * (deltaCost) / temperature)) {
                            total_cost = new_total_cost;
                        } else {
                            topdie_inst1->swap_tob_with(topdie_inst2);
                        }
                    }
                } else {
                    auto topdie_inst = this->randomly_choise_one_topdie_insts(topdies);
                    auto prev_tob = topdie_inst->tob();
                    auto next_tob_option = interposer->get_a_idle_tob();
                    if (!next_tob_option.has_value()) {
                        continue; 
                    }
                    auto next_tob = *next_tob_option;

                    for (auto net : topdie_inst->nets()) {
                        new_total_cost -= this->net_cost(net);
                    }                

                    topdie_inst->move_to_tob(next_tob);

                    for (auto net : topdie_inst->nets()) {
                        new_total_cost += this->net_cost(net);
                    }  

                    auto deltaCost = new_total_cost - total_cost;
                    if (deltaCost <= 0) {
                        total_cost = new_total_cost;
                        improved = true;
                    } else {
                        if (random() <= std::exp(-1.0 * (deltaCost) / temperature)) {
                            total_cost = new_total_cost;
                        } else {
                            topdie_inst->move_to_tob(prev_tob);
                        }
                    }
                }   
            }
            
            if (total_cost < best_cost) {
                best_cost = total_cost;
                best_solution = this->save_current_placement(topdies);
                no_improvement_count = 0;
                debug::info_fmt("Finding a better solution, cost: {}", best_cost);
            } else {
                no_improvement_count++;
            }
            
            temperature = this->calculate_next_temperature(temperature, iteration);
            iteration++;
            
            if (iteration % 10 == 0) {
                debug::info_fmt("迭代 {}: 温度 = {:.2f}, 当前代价 = {}, 最佳代价 = {}", 
                    iteration, temperature, total_cost, best_cost);
            }
        }
        
        this->restore_placement(topdies, best_solution);
        debug::info_fmt("模拟退火布局完成，最终代价: {}, 总迭代次数: {}", best_cost, iteration);
    }

    auto SAPlaceStrategy::total_net_cost(const std::HashSet<circuit::Net*>& nets) const -> std::i64 {
        std::i64 total = 0;
        for (auto net : nets) {
            total += this->net_cost(net);
        }
        return total;
    }

    // Half weekly line length
    auto SAPlaceStrategy::net_cost(circuit::Net* net) const -> std::i64 {
        auto coords = net->coords();
        debug::check(coords.size() != 0, "no coords in net");

        auto min_row = coords[0].row;
        auto max_row = coords[0].row;

        auto min_col = coords[0].col;
        auto max_col = coords[0].col;

        for (std::usize i = 1; i < coords.size(); ++i) {
            min_row = std::min(min_row, coords[i].row);
            max_row = std::max(max_row, coords[i].row);
        
            min_col = std::min(min_col, coords[i].col);
            max_col = std::max(max_col, coords[i].col); 
        }

        return (max_row - min_row) + (max_col - min_col);
    }
    
    auto SAPlaceStrategy::congestion_cost(hardware::Interposer* interposer) const -> std::i64 {
        return 0; 
    }
    
    auto SAPlaceStrategy::manhattan_distance(
        const hardware::TOBCoord& coord1, 
        const hardware::TOBCoord& coord2
    ) const -> std::i64 {
        return std::abs(coord1.row - coord2.row) + std::abs(coord1.col - coord2.col);
    }
    
    auto SAPlaceStrategy::calculate_next_temperature(double current_temp, std::size_t iteration) const -> double {
        return current_temp * this->_cooling_rate;
    }
    
    auto SAPlaceStrategy::save_current_placement(const std::Vector<circuit::TopDieInstance>& topdies) const 
        -> std::HashMap<circuit::TopDieInstance*, hardware::TOB*> {
        std::HashMap<circuit::TopDieInstance*, hardware::TOB*> placement;
        
        for (const auto& topdie : topdies) {
            placement[const_cast<circuit::TopDieInstance*>(&topdie)] = topdie.tob();
        }
        
        return placement;
    }
    
    auto SAPlaceStrategy::restore_placement(
        std::Vector<circuit::TopDieInstance>& topdies,
        const std::HashMap<circuit::TopDieInstance*, hardware::TOB*>& placement
    ) const -> void {
        for (auto& topdie : topdies) {
            auto it = placement.find(const_cast<circuit::TopDieInstance*>(&topdie));
            if (it != placement.end()) {
                topdie.move_to_tob(it->second);
            }
        }
    }

    auto SAPlaceStrategy::randomly_choise_one_topdie_insts(
        std::Vector<circuit::TopDieInstance>& topdies
    ) const -> circuit::TopDieInstance* {
        auto index = random_i64(0, topdies.size());
        return &topdies.at(index);
    }

    auto SAPlaceStrategy::randomly_choise_two_topdie_insts(
        std::Vector<circuit::TopDieInstance>& topdies
    ) const -> std::Tuple<circuit::TopDieInstance*, circuit::TopDieInstance*> {
        auto topdie_inst1 = this->randomly_choise_one_topdie_insts(topdies);
        circuit::TopDieInstance* topdie_inst2 = nullptr;
        do {
            topdie_inst2 = this->randomly_choise_one_topdie_insts(topdies);
        } while (topdie_inst2 == topdie_inst1);

        return { topdie_inst1, topdie_inst2 };
    }

    auto SAPlaceStrategy::decide_to_swap_topdie_inst() const -> bool {
        return random_i64(0, 10) > 3;
    }
}
