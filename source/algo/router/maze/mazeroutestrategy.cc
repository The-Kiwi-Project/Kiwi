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
#include <algorithm>

#include <cassert>
#include <type_traits>

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
        try{
            // three: [bump_to_bump, track_to_bump, bump_to_track]
            debug::debug("Maze routing for synchronized nets");
            std::Array<std::Vector<algo::RerouteStrategy::routed_path>, 3> three_paths {};
            std::Array<std::Vector<std::Option<hardware::Bump*>>, 3> three_end_bumps {};
            std::Array<std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>, 3> three_end_track_to_tob_maps {};
            std::HashSet<hardware::Track*> occupied_tracks_vec {}; // 某些线的头尾track已经被占用了
            std::usize max_length {0};

            if (ptr_sync_net->bttnets().size() > 0){
                for (auto& net: ptr_sync_net->bttnets()){
                    occupied_tracks_vec.emplace(net->end_track());
                }
            }
            if (ptr_sync_net->ttbnets().size() > 0){
                for (auto& net: ptr_sync_net->ttbnets()){
                    occupied_tracks_vec.emplace(net->begin_track());
                }
            }

            if (ptr_sync_net->btbnets().size() > 0){
                auto current_len = sync_preroute<circuit::BumpToBumpNet>(
                    ptr_interposer, ptr_sync_net->btbnets(),
                    three_paths[0], three_end_bumps[0], three_end_track_to_tob_maps[0],
                    occupied_tracks_vec
                );
                max_length = current_len > max_length ? current_len : max_length;
            }
            if (ptr_sync_net->ttbnets().size() > 0){
                auto current_len = sync_preroute<circuit::TrackToBumpNet>(
                    ptr_interposer, ptr_sync_net->ttbnets(),
                    three_paths[1], three_end_bumps[1], three_end_track_to_tob_maps[1],
                    occupied_tracks_vec
                );
                max_length = current_len > max_length ? current_len : max_length;
            }
            if (ptr_sync_net->bttnets().size() > 0){
                auto current_len = sync_preroute<circuit::BumpToTrackNet>(
                    ptr_interposer, ptr_sync_net->bttnets(),
                    three_paths[2], three_end_bumps[2], three_end_track_to_tob_maps[2],
                    occupied_tracks_vec
                );
                max_length = current_len > max_length ? current_len : max_length;
            }

            while (true){
                debug::debug("Route BumpToBump Synchronized Net");
                auto [success, ml] = sync_reroute(
                    ptr_interposer, three_paths[0], three_end_bumps[0], three_end_track_to_tob_maps[0],
                    2, max_length
                );
                if (success){
                    debug::debug("Route TrackToBump Synchronized Net");
                    auto [success, ml] = sync_reroute(
                        ptr_interposer, three_paths[1], three_end_bumps[1], three_end_track_to_tob_maps[1],
                        1, max_length
                    );
                    if (success){
                        debug::debug("Route BumpToTrack Synchronized Net");
                        auto [success, ml] = sync_reroute(
                            ptr_interposer, three_paths[2], three_end_bumps[2], three_end_track_to_tob_maps[2],
                            1, max_length
                        );
                        max_length = ml;
                        if (success){
                            break;
                        }
                        else{
                            continue;
                        }
                    }
                    else{
                        max_length = ml;
                        continue;
                    }
                }
                else{
                    max_length = ml;
                    continue;
                }
            }
        }
        catch (const std::exception& e){
            throw std::runtime_error(std::format("MazeRouteStrategy::route_sync_net: {}", std::String(e.what())));
        }
    }

    auto MazeRouteStrategy::check_found(
        const std::HashSet<hardware::Track*>& end_tracks,
        hardware::Track* track
    ) const -> bool{
        bool found = false;
        for (auto& t: end_tracks){
            if (t->coord() == track->coord()){
                found = true;
                break;
            }
        }
        return found;
    }

    // Return : Vector<(Track*, COBConnector)>
    auto MazeRouteStrategy::search_path(
        hardware::Interposer* interposer, 
        const std::HashSet<hardware::Track*>& begin_tracks,
        const std::HashSet<hardware::Track*>& end_tracks,
        const std::HashSet<hardware::Track*>& occupied_tracks
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

            // if (end_tracks.find(track) != end_tracks.end()) {    
            if (check_found(end_tracks, track)) {
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
                if (prev_track_infos.find(next_track) != prev_track_infos.end() || occupied_tracks.contains(next_track)) {
                    continue;
                }

                queue.push(next_track);             
                prev_track_infos.insert({
                    next_track, 
                    std::Tuple<Track*, COBConnector>{track, connector}
                });
            }
        }

        throw std::runtime_error("MazeRouteStrategy::search_path() >> path not found");
    }

    auto MazeRouteStrategy::route_path(
        hardware::Interposer* interposer, 
        const std::HashSet<hardware::Track*>& begin_tracks,
        const std::HashSet<hardware::Track*>& end_tracks
    ) const -> std::Vector<hardware::Track*> 
    try {
        std::HashSet<hardware::Track*> empty {};
        auto path_info = search_path(interposer, begin_tracks, end_tracks, empty);

        auto path = std::Vector<hardware::Track*>{};

        hardware::Track* prev_track = nullptr;      // the track after this
        for (auto iter = path_info.rbegin(); iter != path_info.rend(); ++iter) {               
            auto [track, connector] = *iter;       
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

    template <class Net>
    auto MazeRouteStrategy::sync_preroute(
            hardware::Interposer* interposer,
            std::Vector<std::Box<Net>>& sync_net,
            std::Vector<algo::RerouteStrategy::routed_path>& paths,
            std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>& end_track_to_tob_maps,
            std::HashSet<hardware::Track*>& occupied_tracks_vec 
        ) const -> std::usize{
        static_assert(
            std::is_same<Net, circuit::BumpToBumpNet>::value ||\
            std::is_same<Net, circuit::TrackToBumpNet>::value ||\
            std::is_same<Net, circuit::BumpToTrackNet>::value,
            "MazeRouteStrategy::sync_preroute() >> Invalid Net type"
        );
        assert (sync_net.size() > 0);

        std::HashSet<hardware::Track*> begin_tracks_set {};
        std::HashSet<hardware::Track*> end_tracks_set {};
        hardware::Bump* begin_bump = nullptr;
        hardware::Bump* end_bump = nullptr;
        std::HashMap<hardware::Track*, hardware::TOBConnector> begin_track_to_tob_map {};
        std::HashMap<hardware::Track*, hardware::TOBConnector> end_track_to_tob_map {};

        for (auto& uptr_net: sync_net){
            auto net = uptr_net.get();
            
            if constexpr (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::BumpToTrackNet>::value){
                begin_bump = net->begin_bump();
                begin_track_to_tob_map = interposer->available_tracks_bump_to_track(begin_bump);
                begin_tracks_set = track_map_to_track_set(begin_track_to_tob_map);
            }
            else if constexpr(std::is_same<Net, circuit::TrackToBumpNet>::value){
                begin_tracks_set = std::HashSet<hardware::Track*>{net->begin_track()};
            }
            if constexpr (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::TrackToBumpNet>::value){
                end_bump = net->end_bump();
                end_track_to_tob_map = interposer->available_tracks_track_to_bump(end_bump);
                end_tracks_set = track_map_to_track_set(end_track_to_tob_map);
            }
            else if constexpr (std::is_same<Net, circuit::BumpToTrackNet>::value){
                end_tracks_set = std::HashSet<hardware::Track*>{net->end_track()};
            }
            
            if (std::is_same<Net, circuit::BumpToBumpNet>::value){
                assert(begin_bump->tob() != end_bump->tob());
            }

            if constexpr(std::is_same<Net, circuit::BumpToTrackNet>::value){
                auto track = net->end_track();
                for (auto& ot: occupied_tracks_vec){
                    if (ot->coord() == track->coord()){
                        occupied_tracks_vec.erase(track);
                        break;
                    }
                }
            }
            if constexpr(std::is_same<Net, circuit::TrackToBumpNet>::value){
                auto track = net->begin_track();
                for (auto& ot: occupied_tracks_vec){
                    if (ot->coord() == track->coord()){
                        occupied_tracks_vec.erase(track);
                        break;
                    }
                }
            }
            auto path_info = search_path(interposer, begin_tracks_set, end_tracks_set, occupied_tracks_vec); // notice: negative sequence
                                                                                        //  |
            auto path = std::Vector<hardware::Track*>{};                                //  V
            hardware::Track* prev_track = nullptr;                                      // prev_track is the track before this
            for (auto iter = path_info.rbegin(); iter != path_info.rend(); ++iter) {               
                auto [track, connector] = *iter;       
                path.emplace_back(track);

                if (connector.has_value()) {
                    connector->connect();                
                }

                if (prev_track != nullptr) {
                    track->set_connected_track(prev_track);
                }

                prev_track = track;     
            }

            if (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::BumpToTrackNet>::value){
                assert (begin_bump != nullptr);

                auto begin_track = path.front();
                assert(begin_track_to_tob_map.contains(begin_track));
                begin_track_to_tob_map.find(begin_track)->second.connect();
                begin_bump->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
            }
            if (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::TrackToBumpNet>::value){
                assert (end_bump != nullptr);

                auto end_track = path.back();
                assert(end_track_to_tob_map.contains(end_track));
                end_track_to_tob_map.find(end_track)->second.connect();
                end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);

                end_bumps.emplace_back(end_bump);
                end_track_to_tob_maps.emplace_back(std::HashMap<hardware::Track*, hardware::TOBConnector>{*end_track_to_tob_map.find(end_track)});
            }

            RerouteStrategy::routed_path reversed_path_info {};
            std::transform(path_info.rbegin(), path_info.rend(), std::back_inserter(reversed_path_info), [](auto& pair) {
                return pair; 
            });
            paths.emplace_back(reversed_path_info);
        }
        std::usize max_length = 0;
        for (auto& path: paths) {
            max_length = max_length < path.size() ? path.size() : max_length;
        }
        if constexpr(std::is_same<Net, circuit::BumpToBumpNet>::value){
            return max_length + 2;
        }
        else if constexpr(std::is_same<Net, circuit::TrackToBumpNet>::value || std::is_same<Net, circuit::BumpToTrackNet>::value){
            return max_length + 1;
        }
    }

    auto MazeRouteStrategy::sync_reroute(
        hardware::Interposer* interposer,
        std::Vector<RerouteStrategy::routed_path>& paths,
        const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
        std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>& end_track_to_tob_maps,
        std::usize bump_extra_length, std::usize max_length
    ) const -> std::tuple<bool, std::usize>{
        std::Vector<algo::RerouteStrategy::routed_path*> nets_to_be_rerouted {};
        std::Vector<std::Option<hardware::Bump*>> related_end_bumps {};
        std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>*> related_maps {};
        for (std::usize i = 0; i < paths.size(); ++i) {
            auto& path = paths[i];
            if (path.size() + bump_extra_length < max_length) {
                nets_to_be_rerouted.emplace_back((&path));
                if (end_bumps.size() > 0){
                    related_end_bumps.emplace_back(end_bumps[i]);
                }
                if (end_track_to_tob_maps.size() > 0){
                    related_maps.push_back(&end_track_to_tob_maps[i]);
                }
            }
        }
        if (nets_to_be_rerouted.size() > 0) {
            auto [success, ml] = _rerouter->reroute(
                interposer, nets_to_be_rerouted, max_length, related_end_bumps, related_maps, bump_extra_length
            );
            
            if (success){   // routing done with ml == max_length
                assert (max_length == ml);
                return std::tuple<std::usize, std::usize>{true, max_length};
            }
            else{   // have longer path OR routing failed
                return std::tuple<std::usize, std::usize>{false, ml};
            }
        }
        else{
            return std::tuple<std::usize, std::usize>{true, max_length};
        }
    }

}