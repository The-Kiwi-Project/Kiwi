#include <std/collection.hh>
#include <std/memory.hh>
#include <std/utility.hh>
#include "debug/debug.hh"
#include <algo/router/maze/mazeroutestrategy.hh>
#include <hardware/interposer.hh>
#include <circuit/net/nets.hh>

#include <iostream>

using namespace kiwi::hardware;
using namespace kiwi::algo;
using namespace kiwi::circuit;
using namespace kiwi;

static void test_route_bump_to_bump_sync_net(){
    debug::debug("*******************test routing bump to bump synchronized net**************************");

    Interposer interposer {};
    MazeRouteStrategy router {};

    // net1
    auto begin_bump1 = interposer.get_bump(0, 0, 0).value();
    auto end_bump1 = interposer.get_bump(0, 3, 0).value();

    // net 2
    auto begin_bump2 = interposer.get_bump(2, 1, 0).value();
    auto end_bump2 = interposer.get_bump(3, 3, 0).value();

    std::Vector<std::Box<BumpToBumpNet>> btbnets {};
    std::Vector<std::Box<BumpToTrackNet>> bttnets {};
    std::Vector<std::Box<TrackToBumpNet>> ttbnets {};

    btbnets.emplace_back(std::move(std::make_unique<BumpToBumpNet>(begin_bump1, end_bump1)));
    btbnets.emplace_back(std::move(std::make_unique<BumpToBumpNet>(begin_bump2, end_bump2)));

    SyncNet snet {std::move(btbnets), std::move(bttnets), std::move(ttbnets)};
    router.route_sync_net(&interposer, &snet);

    debug::debug("routing DONE");
    debug::debug("show net1 path:");
    auto begin_track1 = begin_bump1->connected_track();
    for (auto p: begin_track1->track_path()) {
        debug::debug_fmt("{}", p->coord());
    }
    debug::debug("show net2 path:");
    auto begin_track2 = begin_bump2->connected_track();
    for (auto p: begin_track2->track_path()) {
        debug::debug_fmt("{}", p->coord());
    }
    debug::debug("******************************************************************");
}
