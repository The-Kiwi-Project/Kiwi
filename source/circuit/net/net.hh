#pragma once

#include <hardware/coord.hh>
#include <std/collection.hh>
#include <std/string.hh>
#include <std/utility.hh>

namespace kiwi::hardware {
    class TOB;
    class Interposer;
}

namespace kiwi::algo {
    class RouteStrategy;
}

namespace kiwi::circuit {

    class Priority {
    public:
        Priority(int value): _value{value} {}
        auto operator > (const Priority& other) const -> bool {
            return this->_value < other._value;
        }
    private:
        int _value;
    };

    struct Net {
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void = 0;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize = 0;
        virtual auto priority() const -> Priority = 0;
        virtual auto coords() const -> std::Vector<hardware::Coord> = 0;

        virtual ~Net() noexcept {}
    };

}