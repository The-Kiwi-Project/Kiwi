#pragma once

#include <circuit/topdieinst/topdieinst.hh>
#include <circuit/export/export.hh>

#include <std/string.hh>
#include <std/utility.hh>
#include <std/format.hh>

namespace kiwi::circuit {

    struct ConnectVDD {};
    struct ConnectGND {};

    struct ConnectExPort {
        ExternalPort* port;
    };

    struct ConnectBump {
        TopDieInstance* inst;
        std::String name;
    };

    using ConnectedPoint = std::Variant<
        ConnectVDD,
        ConnectGND,
        ConnectExPort, 
        ConnectBump
    >;

    class Pin {
    private:
        static constexpr std::usize VDD_INDEX = 0;
        static constexpr std::usize GND_INDEX = 1;
        static constexpr std::usize EXPORT_INDEX = 2;
        static constexpr std::usize BUMP_INDEX = 3;

    public:
        static auto connect_vdd() -> Pin;
        static auto connect_gnd() -> Pin;
        static auto connect_export(ExternalPort* port) -> Pin; 
        static auto connect_bump(TopDieInstance* inst, std::String name) -> Pin;

    public:
        auto is_vdd() const -> bool;
        auto is_gnd() const -> bool;
        auto is_fixed() const -> bool;
        auto is_external_port() const -> bool;
        auto is_bump() const -> bool;

    public:
        auto to_connect_export() const -> const ConnectExPort&;
        auto to_connect_bump() const -> const ConnectBump&;

    public:
        auto connected_point() const -> const ConnectedPoint & 
        { return this->_point; }

    private:
        Pin(ConnectedPoint point);
        ConnectedPoint _point {};
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
        return std::match(pin.connected_point(), 
            [&ctx](const kiwi::circuit::ConnectVDD& vdd) {
                return std::vformat_to(
                    ctx.out(),
                    std ::string {"vdd"},
                    std ::make_format_args()
                );
            },
            [&ctx](const kiwi::circuit::ConnectGND& gnd) {
                return std::vformat_to(
                    ctx.out(),
                    std ::string {"gnd"},
                    std ::make_format_args()
                );
            },
            [&ctx](const kiwi::circuit::ConnectExPort& eport) {
                return std::vformat_to(
                    ctx.out(),
                    std ::string {"{}"},
                    std ::make_format_args(eport.port->name())
                );
            },
            [&ctx](const kiwi::circuit::ConnectBump& bump) {
                auto s = bump.inst->name();
                return std::vformat_to(
                    ctx.out(),
                    std ::string {"{}.{}"},
                    std ::make_format_args(s, bump.name)
                );
            }
        );
    }
};