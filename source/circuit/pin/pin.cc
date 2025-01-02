#include "./pin.hh"

namespace kiwi::circuit {

    auto connect_export(std::String name) -> Pin {
        return Pin {
            ConnectExPort {
                .name = std::move(name)
            }
        };
    }

    auto connect_bump(TopDieInstance* inst, std::String name) -> Pin {
        return Pin {
            ConnectBump {
                .inst = inst,
                .name = std::move(name)
            }
        };
    }

}