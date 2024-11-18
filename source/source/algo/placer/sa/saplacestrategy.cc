#include "./saplacestrategy.hh"
#include "debug/debug.hh"

#include <algorithm>
#include <hardware/interposer.hh>

#include <circuit/net/net.hh>
#include <circuit/topdie/topdie.hh>
#include <circuit/topdie/topdieinst.hh>

#include <utility/random.hh>

#include <std/collection.hh>
#include <std/integer.hh>
#include <std/math.hh>

namespace kiwi::algo {

    auto SAPlaceStrategy::place(
        hardware::Interposer* interposer,
        std::Vector<circuit::TopDieInstance>& topdies
    ) const -> void {

        auto nets = std::HashSet<circuit::Net*>{};
        for (const auto& topdie_inst : topdies) {
            for (auto net : topdie_inst.nets()) {
                nets.emplace(net);
            }
        }

        double temperature {this->_init_temperature}; 
        auto total_cost = std::i64{0};
        for (auto net : nets) {
            total_cost += this->net_cost(net);
        }

        while (temperature > this->_freeze_temperature) {
            for (std::usize n = 0; n < this->_solve_number; ++n) {
                int new_total_cost = total_cost;

                // Swap two dies or move a die
                if (this->decide_to_swap_topdie_inst()) {
                    auto [topdie_inst1, topdie_inst2] = this->randomly_choise_two_topdie_insts(topdies);
                    
                    // Record chanaged net
                    // Sub their cost first
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

                    // Swap topdie_inst1 and inst2
                    topdie_inst1->swap_tob_with(topdie_inst2);

                    // Add changed nets' cost to new_total_cost
                    for (auto net : changed_nets) {
                        new_total_cost += this->net_cost(net);
                    }

                    auto deltaCost = new_total_cost - total_cost;
                    if (deltaCost <= 0) {
                        total_cost = new_total_cost;
                    } else {
                        if (random() <= std::exp(-1.0 * (deltaCost) / temperature)) {
                            total_cost = new_total_cost;
                        } else {
                            topdie_inst1->swap_tob_with(topdie_inst2);
                        }
                    }


                } else {
                    // Move a dieinst to empty tob
                    auto topdie_inst = this->randomly_choise_one_topdie_insts(topdies);
                    auto prev_tob = topdie_inst->tob();
                    auto next_tob_option = interposer->get_a_idle_tob();
                    if (!next_tob_option.has_value()) {
                        return;
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
                    } else {
                        if (random() <= std::exp(-1.0 * (deltaCost) / temperature)) {
                            total_cost = new_total_cost;
                        } else {
                            topdie_inst->move_to_tob(prev_tob);
                        }
                    }
                }   
            }

            temperature *= 0.9;
        }
    }


    auto SAPlaceStrategy::net_cost(circuit::Net* net) const -> std::i64 {
        auto coords = net->coords();
        debug::check(coords.size() != 0, "no coords in net");

        auto min_row = coords[0].row;
        auto max_row = coords[0].row;

        auto min_col = coords[0].col;
        auto max_col = coords[0].col;

        for (std::usize i = 0; i < coords.size(); ++i) {
            min_row = std::min(min_row, coords[1].row);
            max_row = std::max(max_row, coords[1].row);
        
            min_col = std::min(min_col, coords[i].col);
            max_col = std::min(max_col, coords[i].col);
        }

        return (max_row - min_row) + (max_col - min_col);
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
        return random_i64(0, 10) > 8;
    }
}