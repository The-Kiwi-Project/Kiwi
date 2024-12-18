#pragma once

#include <circuit/net/nets.hh>

#include <std/collection.hh>
#include <std/memory.hh>


namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::circuit {
    class BumpToBumpNet;
    class TrackToBumpNet;
    class BumpToTrackNet;

    class BumpToBumpsNet;
    class TrackToBumpsNet;
    class BumpToTracksNet;

    class TracksToBumpsNet;

    class SyncNet;
}

namespace kiwi::algo {

    struct RerouteStrategy;

    struct RouteStrategy {
        virtual auto route_bump_to_bump_net(hardware::Interposer*, circuit::BumpToBumpNet*) const -> void = 0;
        virtual auto route_track_to_bump_net(hardware::Interposer*, circuit::TrackToBumpNet*) const -> void = 0;
        virtual auto route_bump_to_track_net(hardware::Interposer*, circuit::BumpToTrackNet*) const -> void = 0;

        virtual auto route_bump_to_bumps_net(hardware::Interposer*, circuit::BumpToBumpsNet*) const -> void = 0;
        virtual auto route_track_to_bumps_net(hardware::Interposer*, circuit::TrackToBumpsNet*) const -> void = 0;
        virtual auto route_bump_to_tracks_net(hardware::Interposer*, circuit::BumpToTracksNet*) const -> void = 0;

        virtual auto route_tracks_to_bumps_net(hardware::Interposer*, circuit::TracksToBumpsNet*) const -> void = 0;
        
        virtual auto route_sync_net(hardware::Interposer*, circuit::SyncNet*) const -> void = 0;
    };

}