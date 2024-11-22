#include "circuit/net/types/btnet.hh"
#include "circuit/net/types/btsnet.hh"
#include "circuit/net/types/tbnet.hh"
#include <std/collection.hh>
#include <std/memory.hh>
#include "debug/debug.hh"
#include "hardware/node/track.hh"
#include "hardware/tob/tobsigdir.hh"
#include "./utilty.hh"
#include <algo/router/maze/mazeroutestrategy.hh>
#include <hardware/interposer.hh>
#include <circuit/net/nets.hh>

#include <iostream>

using namespace kiwi::hardware;
using namespace kiwi::algo;
using namespace kiwi::circuit;
using namespace kiwi;

static void test_route_bump_to_bump_net() {
    debug::debug("test_route_bump_to_bump_net");

    auto i = Interposer{};
    auto router = MazeRouteStrategy{};

    auto begin_bump = i.get_bump(0, 0, 45).value();
    auto end_bump = i.get_bump(0, 1, 4).value();
    auto net1 = BumpToBumpNet{begin_bump, end_bump};

    router.route_bump_to_bump_net(&i, &net1);

    Track* track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    auto path1 = track->track_path();
    for (auto t : path1) {
        debug::info_fmt("{}", t->coord());
    }

    std::cout << "====================================" << std::endl;

    begin_bump = i.get_bump(3, 2, 125).value();
    end_bump = i.get_bump(2, 1, 39).value();
    auto net2 = BumpToBumpNet{begin_bump, end_bump};

    router.route_bump_to_bump_net(&i, &net2);

    track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    auto path2 = track->track_path();
    for (auto t : path2) {
        debug::info_fmt("{}", t->coord());
    }

    std::cout << "====================================" << std::endl;

    begin_bump = i.get_bump(3, 2, 34).value();
    end_bump = i.get_bump(2, 3, 78).value();
    auto net3 = BumpToBumpNet{begin_bump, end_bump};

    router.route_bump_to_bump_net(&i, &net3);

    track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    auto path3 = track->track_path();
    for (auto t : path2) {
        debug::info_fmt("{}", t->coord());
    }

}

static void test_route_track_to_bump_net() {
    debug::debug("test_route_track_to_bump_net");

    auto i = Interposer{};
    auto router = MazeRouteStrategy{};

    auto begin_track = i.get_track(0, 0, TrackDirection::Vertical, 45).value();
    auto end_bump = i.get_bump(0, 1, 4).value();
    auto net1 = TrackToBumpNet{begin_track, end_bump};

    router.route_track_to_bump_net(&i, &net1);

    ASSERT(end_bump->signal_dir() == TOBSignalDirection::TrackToBump);

    auto path1 = begin_track->track_path();
    for (auto t : path1) {
        debug::info_fmt("{}", t->coord());
    }

    std::cout << "====================================" << std::endl;

    begin_track = i.get_track(4, 9, TrackDirection::Vertical, 45).value();
    end_bump = i.get_bump(2, 3, 117).value();
    auto net2 = TrackToBumpNet{begin_track, end_bump};

    router.route_track_to_bump_net(&i, &net2);

    ASSERT(end_bump->signal_dir() == TOBSignalDirection::TrackToBump);

    auto path2 = begin_track->track_path();
    for (auto t : path2) {
        debug::info_fmt("{}", t->coord());
    }

    std::cout << "====================================" << std::endl;

    begin_track = i.get_track(4, 9, TrackDirection::Vertical, 45).value();
    end_bump = i.get_bump(1, 1, 25).value();
    auto net3 = TrackToBumpNet{begin_track, end_bump};

    router.route_track_to_bump_net(&i, &net3);

    ASSERT(end_bump->signal_dir() == TOBSignalDirection::TrackToBump);

    auto path3 = begin_track->track_path();
    for (auto t : path3) {
        debug::info_fmt("{}", t->coord());
    }

}

static void test_route_bump_to_track_net() {
    debug::debug("test_route_bump_to_track_net");

    auto i = Interposer{};
    auto router = MazeRouteStrategy{};

    auto begin_bump = i.get_bump(2, 0, 0).value();
    auto end_track = i.get_track(1, 3, TrackDirection::Horizontal, 0).value();
    auto net1 = BumpToTrackNet{begin_bump, end_track};

    router.route_bump_to_track_net(&i, &net1);

    auto begin_track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    auto path1 = begin_track->track_path();
    for (auto t : path1) {
        debug::info_fmt("{}", t->coord());
    }

    std::cout << "====================================" << std::endl;

    begin_bump = i.get_bump(3, 3, 42).value();
    end_track = i.get_track(1, 2, TrackDirection::Vertical, 117).value();
    auto net2 = BumpToTrackNet{begin_bump, end_track};

    router.route_bump_to_track_net(&i, &net2);

    begin_track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    auto path2 = begin_track->track_path();
    for (auto t : path2) {
        debug::info_fmt("{}", t->coord());
    }

    std::cout << "====================================" << std::endl;

    begin_bump = i.get_bump(3, 3, 7).value();
    end_track = i.get_track(5, 11, TrackDirection::Horizontal, 39).value();
    auto net3 = BumpToTrackNet{begin_bump, end_track};

    router.route_bump_to_track_net(&i, &net3);

    begin_track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    auto path3 = begin_track->track_path();
    for (auto t : path3) {
        debug::info_fmt("{}", t->coord());
    }
}

static void test_route_bump_to_bumps_net() {
    debug::debug("test_route_bump_to_bumps_net");

    auto i = Interposer{};
    auto router = MazeRouteStrategy{};

    auto begin_bump = i.get_bump(0, 1, 4).value();
    auto end_bumps = std::Vector<Bump*> {
        i.get_bump(1, 1, 97).value(),
        i.get_bump(2, 2, 12).value(),
        i.get_bump(3, 3, 33).value(),
    };
    
    i.get_track(0, 0, TrackDirection::Vertical, 45);
    auto net1 = BumpToBumpsNet{begin_bump, std::move(end_bumps)};

    router.route_bump_to_bumps_net(&i, &net1);

    auto begin_track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);

    // auto path1 = begin_track->track_path();
    // for (auto t : path1) {
    //     debug::info_fmt("{}", t->coord());
    // }
}

static void test_route_track_to_bumps_net() {
    debug::debug("test_route_track_to_bumps_net");

    auto i = Interposer{};
    auto router = MazeRouteStrategy{};

    auto begin_track = i.get_track(0, 11, TrackDirection::Horizontal, 39).value();
    auto end_bumps = std::Vector<Bump*> {
        i.get_bump(1, 1, 97).value(),
        i.get_bump(2, 2, 12).value(),
        i.get_bump(3, 3, 33).value(),
    };
    
    i.get_track(0, 0, TrackDirection::Vertical, 45);
    auto net1 = TrackToBumpsNet{begin_track, std::move(end_bumps)};

    router.route_track_to_bumps_net(&i, &net1);
}

static void test_route_bump_to_tracks_net() {
    debug::debug("test_route_bump_to_tracks_net");

    auto i = Interposer{};
    auto router = MazeRouteStrategy{};

    auto begin_bump = i.get_bump(0, 1, 4).value();
    auto end_tracks = std::Vector<Track*> {
        i.get_track(0, 11, TrackDirection::Horizontal, 11).value(),
        // i.get_track(0, 11, TrackDirection::Horizontal, 12),
        // i.get_track(0, 11, TrackDirection::Horizontal, 13),
    };
    
    // i.get_track(0, 0, TrackDirection::Vertical, 45);
    auto net1 = BumpToTracksNet{begin_bump, std::move(end_tracks)};

    router.route_bump_to_tracks_net(&i, &net1);

    auto begin_track = begin_bump->connected_track();
    ASSERT(begin_bump->signal_dir() == TOBSignalDirection::BumpToTrack);
}

static void test_route_bump_to_bump_sync_net(){
    debug::debug("*******************test routing bump to bump synchronized net**************************");

    Interposer interposer {};
    MazeRouteStrategy router {};

    // net1
    auto begin_bump1 = interposer.get_bump(0, 0, 0).value();
    auto end_bump1 = interposer.get_bump(0, 3, 10).value();

    // net 2
    auto begin_bump2 = interposer.get_bump(2, 1, 0).value();
    auto end_bump2 = interposer.get_bump(2, 3, 0).value();

    // net 3
    auto begin_bump3 = interposer.get_bump(2, 0, 0).value();
    auto end_track3 = interposer.get_track(1, 2, hardware::TrackDirection::Horizontal, 10).value();

    // net 4
    auto begin_track4 = interposer.get_track(1, 2, hardware::TrackDirection::Horizontal, 0).value();
    auto end_bump4 = interposer.get_bump(3, 3, 0).value();

    std::Vector<std::Box<BumpToBumpNet>> btbnets {};
    std::Vector<std::Box<BumpToTrackNet>> bttnets {};
    std::Vector<std::Box<TrackToBumpNet>> ttbnets {};

    btbnets.emplace_back(std::move(std::make_unique<BumpToBumpNet>(begin_bump1, end_bump1)));
    btbnets.emplace_back(std::move(std::make_unique<BumpToBumpNet>(begin_bump2, end_bump2)));
    bttnets.emplace_back(std::move(std::make_unique<BumpToTrackNet>(begin_bump3, end_track3)));
    ttbnets.emplace_back(std::move(std::make_unique<TrackToBumpNet>(begin_track4, end_bump4)));

    SyncNet snet {std::move(btbnets), std::move(bttnets), std::move(ttbnets)};
    router.route_sync_net(&interposer, &snet);

    debug::debug("routing DONE");
    debug::debug("show net1 path:(length:)");
    auto begin_track1 = begin_bump1->connected_track();
    debug::debug_fmt("length: {}", begin_track1->track_path().size() + 2);
    for (auto p: begin_track1->track_path()) {
        debug::debug_fmt("{}", p->coord());
    }
    debug::debug("show net2 path:");
    auto begin_track2 = begin_bump2->connected_track();
    debug::debug_fmt("length: {}", begin_track2->track_path().size() + 2);
    for (auto p: begin_track2->track_path()) {
        debug::debug_fmt("{}", p->coord());
    }
    debug::debug("show net3 path:");
    auto begin_track3 = begin_bump3->connected_track();
    debug::debug_fmt("length: {}", begin_track3->track_path().size() + 1);
    for (auto p: begin_track3->track_path()) {
        debug::debug_fmt("{}", p->coord());
    }
    debug::debug("show net4 path:");
    debug::debug_fmt("length: {}", begin_track4->track_path().size() + 1);
    for (auto p: begin_track4->track_path()) {
        debug::debug_fmt("{}", p->coord());
    }
    debug::debug("******************************************************************");
}

void test_router_main() {
    test_route_bump_to_bump_net();
    test_route_track_to_bump_net();
    test_route_bump_to_track_net();

    test_route_bump_to_bumps_net();
    test_route_track_to_bumps_net();
    test_route_bump_to_tracks_net();
    // test_route_bump_to_bump_sync_net();
}