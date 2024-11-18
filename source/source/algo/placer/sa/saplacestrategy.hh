#pragma once

#include "../placestrategy.hh"
#include <std/utility.hh>
#include <std/integer.hh>

namespace kiwi::circuit {
    class TopDieInstance;
    class Net;
}

namespace kiwi::algo {

    class SAPlaceStrategy : public PlaceStrategy {
    public:
        virtual auto place(
            hardware::Interposer* interposer,
            std::Vector<circuit::TopDieInstance>& topdies
        ) const -> void override;

    private:
        auto net_cost(circuit::Net* net) const -> std::i64;
        auto randomly_choise_one_topdie_insts(std::Vector<circuit::TopDieInstance>& topdies) const -> circuit::TopDieInstance*;
        auto randomly_choise_two_topdie_insts(std::Vector<circuit::TopDieInstance>& topdies) const -> std::Tuple<circuit::TopDieInstance*, circuit::TopDieInstance*>;
        auto decide_to_swap_topdie_inst() const -> bool;

    private:
        const double _init_temperature {10.0};
        const double _freeze_temperature {2.0};
        const std::size_t _solve_number {10};
    };

}