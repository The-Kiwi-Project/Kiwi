#pragma once

#include <hardware/interposer.hh>

#include <std/collection.hh>
#include <std/utility.hh>


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

    struct RouteStrategy {
        virtual auto route_bump_to_bump_net(hardware::Interposer*, circuit::BumpToBumpNet*) const -> std::usize = 0;
        virtual auto route_track_to_bump_net(hardware::Interposer*, circuit::TrackToBumpNet*) const -> std::usize = 0;
        virtual auto route_bump_to_track_net(hardware::Interposer*, circuit::BumpToTrackNet*) const -> std::usize = 0;

        virtual auto route_bump_to_bumps_net(hardware::Interposer*, circuit::BumpToBumpsNet*) const -> std::usize = 0;
        virtual auto route_track_to_bumps_net(hardware::Interposer*, circuit::TrackToBumpsNet*) const -> std::usize = 0;
        virtual auto route_bump_to_tracks_net(hardware::Interposer*, circuit::BumpToTracksNet*) const -> std::usize = 0;

        virtual auto route_tracks_to_bumps_net(hardware::Interposer*, circuit::TracksToBumpsNet*) const -> std::usize = 0;
        
        virtual auto route_sync_net(hardware::Interposer*, circuit::SyncNet*) const -> std::usize = 0;
    };
    
}