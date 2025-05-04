#include "./route.hh"
#include <algo/router/routeerror.hh>
#include "debug/debug.hh"
#include <circuit/basedie.hh>
#include <hardware/interposer.hh>

namespace kiwi::algo {

    auto route_nets(
        hardware::Interposer* interposer,
        circuit::BaseDie* basedie,
        const RouteStrategy& strateg
    ) -> std::usize 
    try {
        debug::info("Route nets");
        auto& nets = basedie->nets();

        debug::debug("Sort by priority");
        std::usize max_port_num {0};
        for (const auto& net : nets) {
            max_port_num = std::max(max_port_num, net->port_number());
        }
        for (auto& net : nets) {
            net->update_priority(0.9*(net->port_number() / max_port_num));
        }
        auto compare = [] (const std::Box<circuit::Net>& n1, const std::Box<circuit::Net>& n2) -> bool {
            return n1->priority() > n2->priority();
        };
        std::sort(nets.begin(), nets.end(), compare);

        // manage resources
        for (auto& net: nets){
            net->check_accessable_cobunit();
        }
        interposer->manage_cobunit_resources();
        
        // route nets
        std::usize total_length {0};
        for (auto& net : nets) {
            try {
                auto length = net->route(interposer, strateg);
                net->set_length(length);
                total_length += length;
            } 
            catch (const RetryExpt& err) {
                // TODO: 改一下异常处理
                
            }
            catch (const FinalError& err){
                debug::exception_in("route_nets()", err.what());
            }
            catch (const std::exception& err){
                throw std::runtime_error("route_nets() >> " + std::String(err.what()));
            }
        }
        return total_length;
    }
    THROW_UP_WITH("algo::router")

    auto show_route_result(std::Vector<std::Box<circuit::Net>>& nets, std::usize total_length) -> std::Tuple<std::usize, std::usize, float> {
        std::usize max_l{0};
        for (auto& n: nets) {
            max_l = max_l > n->length() ? max_l : n->length();
        }

        float sync_l{0.0}, sync_num{0.0};
        for (auto& n: nets) {
            if (n->sync_length() != 0) {
                sync_l += n->sync_length();
                sync_num += n->net_size();
            }
        }
        debug::info_fmt(
            "Length: {}, Max Length: {}, SyncNet Average Length: {}",\
            total_length, max_l, sync_l / sync_num
        );

        return std::Tuple<std::usize, std::usize, float>{total_length, max_l, sync_l / sync_num};
    }

}