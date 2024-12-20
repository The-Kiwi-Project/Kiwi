#include "cntpin.hh"

namespace kiwi::circuit {

    auto connect_export(std::String name) -> ConnectionPin {
        return ConnectExPort {
            ConnectExPort {
                .name = std::move(name)
            }
        };
    }

    auto connect_bump(TopDieInstance* inst, std::String name) -> ConnectionPin {
        return ConnectionPin {
            ConnectBump {
                .inst = inst,
                .name = std::move(name)
            }
        };
    }

}