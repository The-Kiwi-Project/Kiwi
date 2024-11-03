#pragma once

#include "../routestrategy.hh"
#include <std/integer.hh>

#include <hardware/cob/cob.hh>
#include <hardware/tob/tob.hh>
#include <hardware/node/bump.hh>
#include <hardware/node/track.hh>

#include <std/collection.hh>
#include <std/utility.hh>


namespace kiwi::algo {

    class MazeRouteStrategy : public RouteStrategy {
    public:
        virtual auto route_bump_to_bump_net(hardware::Interposer*, circuit::BumpToBumpNet*)  const -> void override;
        virtual auto route_track_to_bump_net(hardware::Interposer*, circuit::TrackToBumpNet*) const -> void override;
        virtual auto route_bump_to_track_net(hardware::Interposer*, circuit::BumpToTrackNet*) const -> void override;

        virtual auto route_bump_to_bumps_net(hardware::Interposer*, circuit::BumpToBumpsNet*)  const -> void override;
        virtual auto route_track_to_bumps_net(hardware::Interposer*, circuit::TrackToBumpsNet*) const -> void override;
        virtual auto route_bump_to_tracks_net(hardware::Interposer*, circuit::BumpToTracksNet*) const -> void override;

        virtual auto route_tracks_to_bumps_net(hardware::Interposer*, circuit::TracksToBumpsNet*) const -> void override;

        virtual auto route_sync_net(hardware::Interposer*, circuit::SyncNet*) const -> void override;

        // virtual auto route_bump_to_bump_sync_net(hardware::Interposer*, circuit::BumpToBumpSyncNet*)  const -> void override;
        // virtual auto route_track_to_bump_sync_net(hardware::Interposer*, circuit::TrackToBumpSyncNet*) const -> void override;
        // virtual auto route_bump_to_track_sync_net(hardware::Interposer*, circuit::BumpToTrackSyncNet*) const -> void override;
    
    private:
        auto search_path(
            hardware::Interposer* interposer, 
            const std::HashSet<hardware::Track*>& begin_tracks,
            const std::HashSet<hardware::Track*>& end_tracks
        ) const -> std::Vector<std::Tuple<hardware::Track*, std::Option<hardware::COBConnector>>>;
    
        auto route_path(
            hardware::Interposer* interposer, 
            const std::HashSet<hardware::Track*>& begin_tracks,
            const std::HashSet<hardware::Track*>& end_tracks
        ) const -> std::Vector<hardware::Track*>;

        auto route_path_with_path_len(
            hardware::Interposer* interposer, 
            const std::HashSet<hardware::Track*>& begin_tracks,
            const std::HashSet<hardware::Track*>& end_tracks,
            std::usize path_len
        ) const -> std::Vector<hardware::Track*>;

        auto track_map_to_track_set(
            const std::HashMap<hardware::Track*, 
            hardware::TOBConnector>& map
        ) const -> std::HashSet<hardware::Track*>;
    };

}
