#pragma once

#include "../routestrategy.hh"
#include "./mazererouter.hh"

#include <hardware/cob/cob.hh>
#include <hardware/tob/tob.hh>
#include <hardware/node/bump.hh>
#include <hardware/node/track.hh>

#include <std/collection.hh>
#include <std/utility.hh>
#include <std/integer.hh>
#include <std/memory.hh>


namespace kiwi::hardware{
    class Bump;
    class Track;
    class COB;
    class COBConnector;
}

namespace kiwi::algo {

    using routed_path = std::Vector<std::Tuple<kiwi::hardware::Track*, std::Option<kiwi::hardware::COBConnector>>>;

    class MazeRouteStrategy : public RouteStrategy {
    public:
        MazeRouteStrategy(): _rerouter(std::make_unique<MazeRerouter>(MazeRerouter{})) {}

    public:
        virtual auto route_bump_to_bump_net(hardware::Interposer*, circuit::BumpToBumpNet*)  const -> std::usize override;
        virtual auto route_track_to_bump_net(hardware::Interposer*, circuit::TrackToBumpNet*) const -> std::usize override;
        virtual auto route_bump_to_track_net(hardware::Interposer*, circuit::BumpToTrackNet*) const -> std::usize override;

        virtual auto route_bump_to_bumps_net(hardware::Interposer*, circuit::BumpToBumpsNet*)  const -> std::usize override;
        virtual auto route_track_to_bumps_net(hardware::Interposer*, circuit::TrackToBumpsNet*) const -> std::usize override;
        virtual auto route_bump_to_tracks_net(hardware::Interposer*, circuit::BumpToTracksNet*) const -> std::usize override;

        virtual auto route_tracks_to_bumps_net(hardware::Interposer*, circuit::TracksToBumpsNet*) const -> std::usize override;

        virtual auto route_sync_net(hardware::Interposer*, circuit::SyncNet*) const -> std::usize override;
    
    private:
        template<class InputNode, class OutputNode>
        auto route_node_to_node_net(hardware::Interposer* interposer, InputNode* input_node, OutputNode* output_node) const -> std::usize;
    
    // simple routing functions
    private:
        auto search_path(
            hardware::Interposer* interposer, 
            const std::Vector<hardware::Track*>& begin_tracks,
            const std::HashSet<hardware::Track*>& end_tracks,
            const std::HashSet<hardware::Track*>& occupied_tracks
        ) const -> std::Vector<std::Tuple<hardware::Track*, std::Option<hardware::COBConnector>>>;
    
        auto route_path(
            hardware::Interposer* interposer, 
            const std::Vector<hardware::Track*>& begin_tracks,
            const std::HashSet<hardware::Track*>& end_tracks
        ) const -> std::Vector<hardware::Track*>;

        auto track_map_to_track_vec(
            const std::HashMap<hardware::Track*, 
            hardware::TOBConnector>& map,
            const std::Array<std::usize, hardware::COB::UNIT_SIZE>& cobunit_usage
        ) const -> std::Vector<hardware::Track*>;

        auto track_map_to_track_set(
            const std::HashMap<hardware::Track*, 
            hardware::TOBConnector>& map
        ) const -> std::HashSet<hardware::Track*>;

        auto check_found(
            const std::HashSet<hardware::Track*>& end_tracks, 
            hardware::Track* track
        ) const -> bool;

    // synchrnized rouitng functions
    private:
        // first round of routing & collecting paths 
        template <class Net>
        auto sync_preroute(
            hardware::Interposer* interposer,
            std::Vector<std::Box<Net>>& sync_net,
            std::Vector<routed_path>& paths,
            std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>& end_track_to_tob_maps,
            std::HashSet<hardware::Track*>& occupied_tracks_vec
        ) const -> std::usize;

        // the path is already stored in ascending order with vector index when return 
        auto sync_reroute(
            hardware::Interposer* interposer,
            std::Vector<routed_path>& paths,
            const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>& end_track_to_tob_maps,
            std::usize bump_length, std::usize max_length
        ) const -> std::tuple<bool, std::usize>;

        auto print_sync_path(circuit::SyncNet*) const -> void;

        template<class InputNode, class OutputNode>
        auto print_path(InputNode* input_node, OutputNode* output_node, const std::Vector<hardware::Track*>& path) const -> void;
    
    private:
        std::Box<MazeRerouter> _rerouter;
    };


}
