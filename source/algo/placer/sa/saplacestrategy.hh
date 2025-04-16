#pragma once

#include "../placestrategy.hh"
#include <std/utility.hh>
#include <std/integer.hh>
#include <debug/debug.hh>
#include <QColor>

namespace kiwi::circuit {
    class TopDieInstance;
    class Net;
    class BaseDie;
    class ExternalPort;
}

namespace kiwi::hardware {
    struct TOBCoord;
    class TOB;
    class COB;
    class Track;
    class Bump;
}

namespace kiwi::algo {
    class RouteStrategy;
    class SAPlaceStrategy : public PlaceStrategy {
    public:
        SAPlaceStrategy() = default;

        SAPlaceStrategy(
            double init_temp = 100.0, 
            double freeze_temp = 0.5, 
            std::size_t solve_num = 50,
            double cooling_rate = 0.95,
            std::size_t max_no_improvement = 50,
            double wirelength_weight = 1.0,
            double congestion_weight = 0.5,
            double thermal_weight = 0.3,
            double power_weight = 0.2
        ) : _init_temperature(init_temp),
            _freeze_temperature(freeze_temp),
            _solve_number(solve_num),
            _cooling_rate(cooling_rate),
            _max_no_improvement(max_no_improvement),
            _wirelength_weight(wirelength_weight),
            _congestion_weight(congestion_weight),
            _thermal_weight(thermal_weight),
            _power_weight(power_weight) {}
            
        virtual auto place(
            hardware::Interposer* interposer,
            std::Vector<circuit::TopDieInstance>& topdies
        ) const -> void override;
        
        virtual auto evaluate_placement(
            hardware::Interposer* interposer,
            const std::Vector<circuit::TopDieInstance>& topdies,
            circuit::BaseDie* basedie
        ) const -> std::i64 override;

    private:
        auto net_cost(circuit::Net* net) const -> std::i64;
        auto total_net_cost(const std::HashSet<circuit::Net*>& nets) const -> std::i64;
        auto congestion_cost(hardware::Interposer* interposer) const -> std::i64;
        auto thermal_cost(const std::Vector<circuit::TopDieInstance>& topdies) const -> std::i64;
        auto power_cost(const std::Vector<circuit::TopDieInstance>& topdies, circuit::BaseDie* basedie) const -> std::i64;
        auto manhattan_distance(const hardware::TOBCoord& coord1, const hardware::TOBCoord& coord2) const -> std::i64;
        
        auto randomly_choice_one_topdie_insts(std::Vector<circuit::TopDieInstance>& topdies) const -> circuit::TopDieInstance*;
        auto randomly_choice_two_topdie_insts(std::Vector<circuit::TopDieInstance>& topdies) const -> std::Tuple<circuit::TopDieInstance*, circuit::TopDieInstance*>;
        auto decide_to_swap_topdie_inst(double temperature) const -> bool;
        
        auto calculate_next_temperature(double current_temp, std::size_t iteration) const -> double;
        
        auto save_current_placement(const std::Vector<circuit::TopDieInstance>& topdies) const -> std::HashMap<circuit::TopDieInstance*, hardware::TOB*>;
        auto restore_placement(
            std::Vector<circuit::TopDieInstance>& topdies,
            const std::HashMap<circuit::TopDieInstance*, hardware::TOB*>& placement
        ) const -> void;
        
        auto collect_nets(const std::Vector<circuit::TopDieInstance>& topdies) const -> std::HashSet<circuit::Net*>;
        
        auto try_routing(
            hardware::Interposer* interposer,
            circuit::BaseDie* basedie,
            const RouteStrategy& route_strategy
        ) const -> std::i64;
        
        auto is_valid_placement(
            hardware::Interposer* interposer,
            const std::Vector<circuit::TopDieInstance>& topdies
        ) const -> bool;
        
        auto get_topdie_power(const circuit::TopDieInstance& topdie) const -> double;
        auto get_topdie_color(const circuit::TopDieInstance& topdie) const -> QColor;

    private:
        const double _init_temperature {100.0}; 
        const double _freeze_temperature {0.5}; 
        const std::size_t _solve_number {50};   
        const double _cooling_rate {0.95};      
        const std::size_t _max_no_improvement {50}; 
        
        const double _wirelength_weight {1.0};  
        const double _congestion_weight {0.5};  
        const double _thermal_weight {0.3};     
        const double _power_weight {0.2};       
        
        const double _thermal_threshold {0.8};  
        const std::i64 _thermal_safe_distance {3}; 
        
        const double _power_threshold {0.7};    
        const std::i64 _power_safe_distance {2}; 
    };
}