#include "circuit/net/types/btnet.hh"
#include "circuit/net/types/btsnet.hh"
#include "circuit/net/types/tbnet.hh"
#include <std/collection.hh>
#include "debug/debug.hh"
#include "hardware/node/track.hh"
#include "hardware/tob/tobsigdir.hh"
#include "./utilty.hh"
#include <algo/router/maze/mazeroutestrategy.hh>
#include <hardware/interposer.hh>
#include <circuit/net/nets.hh>
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

    auto begin_bump = i.get_bump(0, 1, 4).value();
    auto end_track = i.get_track(0, 0, TrackDirection::Vertical, 45).value();
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

void test_router_main() {
    test_route_bump_to_bump_net();
    test_route_track_to_bump_net();
    test_route_bump_to_track_net();

    test_route_bump_to_bumps_net();
    test_route_track_to_bumps_net();
    test_route_bump_to_tracks_net();
}