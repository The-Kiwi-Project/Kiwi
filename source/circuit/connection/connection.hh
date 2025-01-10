#pragma once

#include "./pin.hh"
#include <format>

namespace kiwi::circuit {

    class BaseDie;

    class Connection {
        friend BaseDie;

    private:
        Connection(int sync, Pin input, Pin output);

        void set_sync(int sync) { this->_sync = sync; }

    public:
        auto sync() const -> int { return this->_sync; }
        auto input_pin() const -> const Pin& { return this->_input; }
        auto output_pin() const -> const Pin& { return this->_output; }
    
        void set_input(Pin input) { this->_input = std::move(input); } 
        void set_output(Pin output) { this->_input = std::move(output); }

    private:
        int _sync;
        Pin _input;
        Pin _output;
    };

}

template <>
struct std::formatter<kiwi::circuit::Connection> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && *it != '}') {
            throw std::format_error("Invalid format");
        }
        return it;
    }
    template<typename FormatContext>
    constexpr auto format(const kiwi::circuit::Connection& c, FormatContext& ctx) const {
        return std::vformat_to(ctx.out(), "[{} => {}]", std ::make_format_args(c.input_pin(), c.output_pin()));
    }
};