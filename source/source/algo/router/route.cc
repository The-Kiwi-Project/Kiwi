#include "./route.hh"
#include "algo/router/routeerror.hh"
#include "debug/debug.hh"
#include <circuit/basedie.hh>

namespace kiwi::algo {

    auto route_nets(
        hardware::Interposer* interposer,
        circuit::BaseDie* basedie,
        const RouteStrategy& strateg
    ) -> void 
    try {
        debug::info("Route nets");

        auto& nets = basedie->nets();

        auto compare = [] (const std::Box<circuit::Net>& n1, const std::Box<circuit::Net>& n2) -> bool {
            return n1->priority() > n2->priority();
        };

        debug::debug("Sort by priority");
        std::sort(nets.begin(), nets.end(), compare);

        for (auto& net : nets) {
            try {
                net->route(interposer, strateg);
            } catch (const RouteError& err) {
                // TODO How to report error
                debug::exception_fmt("Route {} failed", static_cast<void*>(net.get()));
            }
        }
    }
    THROW_UP_WITH("Route nets")

}