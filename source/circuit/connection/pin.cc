#include "./pin.hh"

namespace kiwi::circuit {

    Pin::Pin(ConnectedPoint point) :
        _point{std::move(point)}
    {
    }

    auto Pin::connect_vdd(std::String name) -> Pin {
        return Pin { ConnectVDD { std::move(name) } };
    }

    auto Pin::connect_gnd(std::String name) -> Pin {
        return Pin { ConnectGND { std::move(name) } };
    }

    auto Pin::connect_export(ExternalPort* port) -> Pin {
        return Pin { ConnectedPoint{ConnectExPort{.port = port}} };
    }

    auto Pin::connect_bump(TopDieInstance* inst, std::String name) -> Pin {
        return Pin { ConnectedPoint{ConnectBump{.inst = inst, .name = std::move(name)}} };
    }

    auto Pin::is_vdd() const -> bool {
        return this->_point.index() == VDD_INDEX;
    }

    auto Pin::is_gnd() const -> bool {
        return this->_point.index() == GND_INDEX;
    }

    auto Pin::is_fixed() const -> bool {
        return this->is_vdd() || this->is_gnd();
    }

    auto Pin::is_external_port() const -> bool {
        return this->_point.index() == EXPORT_INDEX;
    }

    auto Pin::is_bump() const -> bool {
        return this->_point.index() == BUMP_INDEX;
    }

    auto Pin::to_connect_export() const -> const ConnectExPort& {
        return std::get<EXPORT_INDEX>(this->_point);
    }

    auto Pin::to_connect_bump() const -> const ConnectBump& {
        return std::get<BUMP_INDEX>(this->_point);
    }

}