#include "../routeerror.hh"
#include "./mazeroutestrategy.hh"

#include <circuit/net/nets.hh>
#include <hardware/cob/cob.hh>
#include <hardware/tob/tob.hh>
#include <hardware/interposer.hh>
#include <hardware/node/track.hh>
#include <hardware/node/bump.hh>

#include <std/integer.hh>
#include <std/collection.hh>
#include <std/utility.hh>
#include <debug/debug.hh>

#include <cassert>

namespace kiwi::algo {

    auto MazeRouteStrategy::route_bump_to_bump_net(hardware::Interposer* interposer, circuit::BumpToBumpNet* net) const -> void {
        debug::debug("Maze routing for bump to bump net");
        
        auto begin_bump = net->begin_bump();
        auto end_bump = net->end_bump();
        debug::check(begin_bump->tob() != end_bump->tob(), "Route bump in the same tob");

        auto begin_tracks = interposer->available_tracks_bump_to_track(begin_bump);
        auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);

        // Route a track path from one of `begin_tracks` to one of `end_tracks`
        // the function will connected the COBConnector
        auto path = this->route_path(interposer, track_map_to_track_set(begin_tracks), track_map_to_track_set(end_tracks));

        auto begin_track = path.front();
        auto end_track = path.back();

        assert(begin_tracks.contains(begin_track));
        assert(end_tracks.contains(end_track));
        
        // Connect the TOB Connector
        begin_tracks.find(begin_track)->second.connect();
        begin_bump->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
        end_tracks.find(end_track)->second.connect();
        end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
    }

    auto MazeRouteStrategy::route_track_to_bump_net(hardware::Interposer* interposer, circuit::TrackToBumpNet* net) const -> void {
        debug::debug("Maze routing for track to bumpt net");
        
        auto begin_track = net->begin_track();
        auto end_bump = net->end_bump();

        auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);

        // Route a track path from one of `begin_tracks` to one of `end_tracks`
        // the function will connected the COBConnector
        auto path = this->route_path(interposer, std::HashSet<hardware::Track*>{begin_track}, track_map_to_track_set(end_tracks));

        auto end_track = path.back();
        assert(end_tracks.contains(end_track));

        // Connect the TOB Connector
        end_tracks.find(end_track)->second.connect();

        end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
    }

    auto MazeRouteStrategy::route_bump_to_track_net(hardware::Interposer* interposer, circuit::BumpToTrackNet* net) const -> void {
        debug::debug("Maze routing for bump to track net");
        
        auto begin_bump = net->begin_bump();
        auto end_track = net->end_track();

        auto begin_tracks = interposer->available_tracks_bump_to_track(begin_bump);

        // Route a track path from one of `begin_tracks` to one of `end_tracks`
        // the function will connected the COBConnector
        auto path = this->route_path(interposer, track_map_to_track_set(begin_tracks), std::HashSet<hardware::Track*>{end_track});

        auto begin_track = path.front();
        assert(begin_tracks.contains(begin_track));

        // Connect the TOB Connector
        begin_bump->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
        begin_tracks.find(begin_track)->second.connect();
    }

    auto MazeRouteStrategy::route_bump_to_bumps_net(hardware::Interposer* interposer, circuit::BumpToBumpsNet* net)  const -> void {
        debug::debug("Maze routing for bump to bumps net");
        
        auto begin_bump = net->begin_bump();
        const auto& end_bumps = net->end_bumps();

        auto begin_tracks_set = std::HashSet<hardware::Track*>{};

        for (auto end_bump : end_bumps) {
            // Target: route begin_bump to end_bump?
            auto begin_tracks = interposer->available_tracks_bump_to_track(begin_bump);
            auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);
            for (auto& [t, _] : begin_tracks) {
                begin_tracks_set.emplace(t);
            }
            
            auto path = this->route_path(interposer, begin_tracks_set, this->track_map_to_track_set(end_tracks));

            for (auto& [t, _] : begin_tracks) {
                begin_tracks_set.erase(t);
            }

            // Get begin and end track in path
            auto begin_track = path.front();
            auto end_track = path.back();

            // Connect the TOB from end track to end_bump
            assert(end_tracks.contains(end_track));

            end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
            end_tracks.find(end_track)->second.connect();

            // Is the begin is from begin_tracks?
            auto find_res = begin_tracks.find(begin_track);
            if (find_res != begin_tracks.end()) {
                // If `begin_track` is in `begin_tracks`, mean we find a track which is should be connected
                // with begin bump!
                begin_bump->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
                find_res->second.connect();
            } else {
                // `begin_track` is not the `begin_tracks`, so we find a track in path to connected with `end_bump`
                // do not need to connect TOB!
            }

            // All track in path can see as `begin_track_set`
            for (auto t : path) {
                begin_tracks_set.emplace(t);
            } 
        }
    }

    auto MazeRouteStrategy::route_track_to_bumps_net(hardware::Interposer* interposer, circuit::TrackToBumpsNet* net) const -> void {
        debug::debug("Maze routing for track to bumps net");
        
        auto begin_track = net->begin_track();
        const auto& end_bumps = net->end_bumps();

        auto begin_tracks_set = std::HashSet<hardware::Track *>{begin_track};

        for (auto end_bump : end_bumps) {
            auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);
            auto path = this->route_path(interposer, begin_tracks_set, this->track_map_to_track_set(end_tracks));

            // Get begin and end track in path
            auto begin_track = path.front();
            auto end_track = path.back();

            // Connect the TOB from end track to end_bump
            assert(end_tracks.contains(end_track));
            end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
            end_tracks.find(end_track)->second.connect();

            // All track in path can see as `begin_track_set`
            for (auto t : path) {
                begin_tracks_set.emplace(t);
            } 
        }
    }

    auto MazeRouteStrategy::route_bump_to_tracks_net(hardware::Interposer* interposer, circuit::BumpToTracksNet* net) const -> void {
        debug::debug("Maze routing for bump to tracks net");

        auto begin_bump = net->begin_bump();
        const auto& end_tracks = net->end_tracks();

        auto begin_tracks_set = std::HashSet<hardware::Track*>{};

        for (auto end_track : end_tracks) {
            auto begin_tracks = interposer->available_tracks_bump_to_track(begin_bump);
            for (auto& [t, _] : begin_tracks) {
                begin_tracks_set.emplace(t);
            }
    
            auto path = this->route_path(interposer, begin_tracks_set, std::HashSet<hardware::Track *>{end_track});

            for (auto& [t, _] : begin_tracks) {
                begin_tracks_set.erase(t);
            }

            // Get begin and end track in path
            auto begin_track = path.front();

            // Is the begin is from begin_tracks?
            auto find_res = begin_tracks.find(begin_track);
            if (find_res != begin_tracks.end()) {
                // If `begin_track` is in `begin_tracks`, mean we find a track which is should be connected
                // with begin bump!
                find_res->second.connect();
                begin_bump->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
            } else {
                // `begin_track` is not the `begin_tracks`, so we find a track in path to connected with `end_bump`
                // do not need to connect TOB!
            }

            // All track in path can see as `begin_track_set`
            for (auto t : path) {
                begin_tracks_set.emplace(t);
            } 
        }
    }

    auto MazeRouteStrategy::route_tracks_to_bumps_net(hardware::Interposer* interposer, circuit::TracksToBumpsNet* net) const -> void {
        debug::debug("Maze routing for tracks to bumps net");
        
        auto& begin_tracks = net->begin_tracks();
        auto& end_bumps = net->end_bumps();

        auto begin_tracks_set = std::HashSet<hardware::Track*>{};
        for (auto t : begin_tracks) {
            begin_tracks_set.emplace(t);
        }

        for (auto end_bump : end_bumps) {
            auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);
            auto path = this->route_path(interposer, begin_tracks_set, track_map_to_track_set(end_tracks));

            auto end_track = path.back();
            assert(end_tracks.contains(end_track));

            for (auto t : path) {
                begin_tracks_set.emplace(t);
            }
        }
    }

    auto MazeRouteStrategy::route_sync_net(hardware::Interposer* ptr_interposer, circuit::SyncNet* ptr_sync_net) const -> void
    {
        debug::debug("Maze routing for sync net");
    }

    // auto MazeRouteStrategy::route_bump_to_bump_sync_net(hardware::Interposer* interposer, circuit::BumpToBumpSyncNet* sync_net)  const -> void {

    // }

    // auto MazeRouteStrategy::route_track_to_bump_sync_net(hardware::Interposer* interposer, circuit::TrackToBumpSyncNet* net) const -> void {

    // }

    // auto MazeRouteStrategy::route_bump_to_track_sync_net(hardware::Interposer* interposer, circuit::BumpToTrackSyncNet* net) const -> void {

    // }

    // Return : Vector<(Track*, COBConnector)>
    auto MazeRouteStrategy::search_path(
        hardware::Interposer* interposer, 
        const std::HashSet<hardware::Track*>& begin_tracks,
        const std::HashSet<hardware::Track*>& end_tracks
    ) const -> std::Vector<std::Tuple<hardware::Track*, std::Option<hardware::COBConnector>>> {
        using namespace hardware;

        auto queue = std::Queue<Track*>{};
        // HashMap<Track, Option<(Track, Connector)>>
        auto prev_track_infos = 
            std::HashMap<Track*, std::Option<std::Tuple<Track*, COBConnector>>>{};

        for (auto& t: begin_tracks) {
            queue.push(t);
            prev_track_infos.insert({t, std::nullopt});
        }
        
        while (!queue.empty()) {
            auto track = queue.front();
            queue.pop();

            if (end_tracks.find(track) != end_tracks.end()) {
                auto path = std::Vector<std::Tuple<Track*, std::Option<COBConnector>>>{};
                auto cur_track = track;
                while (true) {
                    auto prev_track_info = prev_track_infos.find(cur_track);
                    assert(prev_track_info != prev_track_infos.end());
                    // Reach start track
                    if (!prev_track_info->second.has_value()) {
                        break;
                    }

                    path.emplace_back(cur_track, std::get<1>(*prev_track_info->second));
                    cur_track = std::get<0>(*prev_track_info->second);
                }
                path.emplace_back(cur_track, std::nullopt);

                return {path};
            }

            for (auto& [next_track, connector] : interposer->adjacent_idle_tracks(track)) {
                if (prev_track_infos.find(next_track) != prev_track_infos.end()) {
                    continue;
                }

                queue.push(next_track);             
                prev_track_infos.insert({
                    next_track, 
                    std::Tuple<Track*, COBConnector>{track, connector}
                });
            }
        }

        throw RouteError {};
    }

    auto MazeRouteStrategy::route_path(
        hardware::Interposer* interposer, 
        const std::HashSet<hardware::Track*>& begin_tracks,
        const std::HashSet<hardware::Track*>& end_tracks
    ) const -> std::Vector<hardware::Track*> 
    try {
        auto path_info = search_path(interposer, begin_tracks, end_tracks);

        auto path = std::Vector<hardware::Track*>{};

        hardware::Track* prev_track = nullptr;
        for (auto iter = path_info.rbegin(); iter != path_info.rend(); ++iter) {               
            auto [track, connector] = *iter;        // BUG: search_path returns with "throw RouteError
            path.emplace_back(track);

            if (connector.has_value()) {
                connector->connect();                
            }

            if (prev_track != nullptr) {
                track->set_connected_track(prev_track);
            }

            prev_track = track;
        }

        return path;
    }
    catch (std::exception& e){
        throw std::runtime_error("MazeRouteStrategy::route_path() >> " + std::String(e.what()));
    }

    auto MazeRouteStrategy::route_path_with_path_len(
        hardware::Interposer* interposer, 
        const std::HashSet<hardware::Track*>& begin_tracks,
        const std::HashSet<hardware::Track*>& end_tracks,
        std::usize path_len
    ) const -> std::Vector<hardware::Track*> {
        using namespace hardware;
        return {};
    }

    auto MazeRouteStrategy::track_map_to_track_set(
        const std::HashMap<hardware::Track*, 
        hardware::TOBConnector>& map
    ) const -> std::HashSet<hardware::Track*> {
        auto set = std::HashSet<hardware::Track*>{};
        for (auto [t, _] : map) {
            set.emplace(t);
        }
        return set;
    }

}