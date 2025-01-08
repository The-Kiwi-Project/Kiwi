#include "./pin.hh"

namespace kiwi::circuit {

    Pin::Pin(ConnectedPoint point) :
        _point{std::move(point)}
    {
    }

    auto Pin::connect_export(ExternalPort* port) -> Pin {
        return Pin { ConnectedPoint{ConnectExPort{.port = port}} };
    }

    auto Pin::connect_bump(TopDieInstance* inst, std::String name) -> Pin {
        return Pin { ConnectedPoint{ConnectBump{.inst = inst, .name = std::move(name)}} };
    }

}