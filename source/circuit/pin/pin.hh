#pragma once

#include "../topdieinst/topdieinst.hh"

#include <std/string.hh>
#include <std/utility.hh>
#include <std/format.hh>

namespace kiwi::circuit {

    struct ConnectExPort {
        std::String name;
    };

    struct ConnectBump {
        TopDieInstance* inst;
        std::String name;
    };

    using Pin = std::Variant<ConnectExPort, ConnectBump>;

    auto connect_export(std::String name) -> Pin;
    auto connect_bump(TopDieInstance* inst, std::String name) -> Pin;

    struct Connection {
        Pin input;
        Pin output;
    };

}

template <>
struct std::formatter<kiwi::circuit::Pin> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        auto end = ctx.end();
        if (it != end && *it != '}') {
            throw std::format_error("Invalid format");
        }
        return it;
    }
    template<typename FormatContext>
    constexpr auto format(const kiwi::circuit::Pin& pin, FormatContext& ctx) const {
        return std::match(pin, 
            [&ctx](const kiwi::circuit::ConnectExPort& eport) {
                return std::vformat_to(
                    ctx.out(),
                    std ::string {"{}"},
                    std ::make_format_args(eport.name)
                );
            },
            [&ctx](const kiwi::circuit::ConnectBump& bump) {
                return std::vformat_to(
                    ctx.out(),
                    std ::string {"{}.{}"},
                    std ::make_format_args(bump.inst->name(), bump.name)
                );
            }
        );
    }
};