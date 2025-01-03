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
        Priority(float value): _value{value} {}
        auto operator > (const Priority& other) const -> bool {
            return this->_value < other._value;
        }
        auto value() const -> float {
            return this->_value;
        }

    private:
        float _value;
    };

    class Net {
    public:
        virtual auto update_tob_postion(hardware::TOB* prev_tob, hardware::TOB* next_tob) -> void = 0;
        virtual auto route(hardware::Interposer* interposer, const algo::RouteStrategy& strategy) -> std::usize = 0;
        virtual auto update_priority(float bias) -> void = 0;
        virtual auto coords() const -> std::Vector<hardware::Coord> = 0;
        virtual auto check_accessable_cobunit() -> void = 0;
        virtual auto port_number() const -> std::usize = 0;
        virtual auto to_string() const -> std::String = 0;

        virtual auto priority() const -> Priority{
            return this->_priority;
        }
    
    public:
        Net(Priority priority): _priority{priority} {}
        virtual ~Net() noexcept {}
    
    protected:
        Priority _priority;
    };

}