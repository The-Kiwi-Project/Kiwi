#include "./mazeroutestrategy.hh"
#include "./path_length.hh"

#include <circuit/net/nets.hh>
#include <hardware/interposer.hh>
#include <algo/router/routeerror.hh>

#include <std/integer.hh>
#include <std/collection.hh>
#include <std/utility.hh>
#include <debug/debug.hh>
#include <algorithm>

#include <cassert>
#include <type_traits>

namespace kiwi::algo {

    auto MazeRouteStrategy::route_bump_to_bump_net(hardware::Interposer* interposer, circuit::BumpToBumpNet* net) const -> std::usize {
        debug::debug("Maze routing for bump to bump net");
        
        auto begin_bump = net->begin_bump();
        auto end_bump = net->end_bump();
        debug::check(begin_bump->tob() != end_bump->tob(), "Route bump in the same tob");

        return this->route_node_to_node_net<hardware::Bump, hardware::Bump>(interposer, begin_bump, end_bump);
    }

    auto MazeRouteStrategy::route_track_to_bump_net(hardware::Interposer* interposer, circuit::TrackToBumpNet* net) const -> std::usize {
        debug::debug("Maze routing for track to bump net");
        
        auto begin_track = net->begin_track();
        auto end_bump = net->end_bump();

        return this->route_node_to_node_net<hardware::Track, hardware::Bump>(interposer, begin_track, end_bump);
    }

    auto MazeRouteStrategy::route_bump_to_track_net(hardware::Interposer* interposer, circuit::BumpToTrackNet* net) const -> std::usize {
        debug::debug("Maze routing for bump to track net");
        
        auto begin_bump = net->begin_bump();
        auto end_track = net->end_track();

        return this->route_node_to_node_net<hardware::Bump, hardware::Track>(interposer, begin_bump, end_track);
    }

    template<class InputNode, class OutputNode>
        auto MazeRouteStrategy::route_node_to_node_net(hardware::Interposer* interposer, InputNode* input_node, OutputNode* output_node) const -> std::usize {
        std::Vector<hardware::Track*> path {};
        std::usize path_l {0};
        std::Vector<hardware::Track*> begin_tracks_vec {};
        std::HashSet<hardware::Track*> end_tracks_set {};
        std::HashMap<kiwi::hardware::Track *, kiwi::hardware::TOBConnector> begin_tracks_map {};
        std::HashMap<kiwi::hardware::Track *, kiwi::hardware::TOBConnector> end_tracks_map {};
        bool is_begin_connector_connected {false};
        bool is_end_connector_connected {false};

        // track node
        if constexpr (std::is_same<InputNode, hardware::Track>::value){
            begin_tracks_vec.emplace_back(input_node);
        }
        if constexpr( std::is_same<OutputNode, hardware::Track>::value){
            end_tracks_set.emplace(output_node);
        }

        // bump node
        if constexpr (std::is_same<InputNode, hardware::Bump>::value){
            // not been connected yet
            if (input_node->connected_track() == nullptr) {
                begin_tracks_map = interposer->available_tracks_bump_to_track(input_node);
                if (begin_tracks_map.empty()){
                    throw RetryExpt(std::format("MazeRouteStrategy::route_node_to_node_net()", "No available tracks for input node {}", input_node->coord()));
                    // debug::exception_in("MazeRouteStrategy::route_node_to_node_net()", "No available tracks for input node");
                }
                begin_tracks_vec = track_map_to_track_vec(begin_tracks_map, input_node->tob()->cobunit_resources());
            }
            // already been connected
            else{
                is_begin_connector_connected = true;
                auto begin_track = input_node->connected_track();
                while (begin_track != nullptr) {
                    begin_tracks_vec.emplace_back(begin_track);
                    begin_track = begin_track->next_track();
                }
            }
        }
        if constexpr (std::is_same<OutputNode, hardware::Bump>::value){
            // not been connected yet
            if (output_node->connected_track() == nullptr) {
                end_tracks_map = interposer->available_tracks_track_to_bump(output_node);
                if (end_tracks_map.empty()){
                    throw RetryExpt(std::format("MazeRouteStrategy::route_node_to_node_net()", "No available tracks for output node {}", output_node->coord()));
                    // debug::exception_in("MazeRouteStrategy::route_node_to_node_net()", "No available tracks for output node");
                }
                end_tracks_set = track_map_to_track_set(end_tracks_map);
            }
            // already been connected
            else{
                is_end_connector_connected = true;
                auto end_track = output_node->connected_track();
                while (end_track != nullptr) {
                    end_tracks_set.emplace(end_track);
                    end_track = end_track->prev_track();
                }
            }
        }
        if (begin_tracks_vec.empty() || end_tracks_set.empty()){
            throw FinalError("MazeRouteStrategy::route_node_to_node_net(): no available begin/end tracks for unknown reasons");
        }

        // route path
        path = this->route_path(interposer, begin_tracks_vec, end_tracks_set);
//!
print_path(input_node, output_node, path);
//!
        auto begin_track = path.front();
        auto end_track = path.back();

        // connect bump to track
        if constexpr(std::is_same<InputNode, hardware::Bump>::value){
            if (!is_begin_connector_connected){
                input_node->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
                begin_tracks_map.find(begin_track)->second.connect();
                path_l += 1;
            }
        }
        if constexpr(std::is_same<OutputNode, hardware::Bump>::value){
            if (!is_end_connector_connected){
                output_node->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
                end_tracks_map.find(end_track)->second.connect();
                path_l += 1;
            }
        }

        path_l += path_length(path);
        return path_l;
    }

    auto MazeRouteStrategy::route_bump_to_bumps_net(hardware::Interposer* interposer, circuit::BumpToBumpsNet* net)  const -> std::usize {
        debug::debug("Maze routing for bump to bumps net");
        
        auto begin_bump = net->begin_bump();
        const auto& end_bumps = net->end_bumps();

        auto begin_tracks_vec = std::Vector<hardware::Track*>{};

        std::usize total_length {0};
        for (auto end_bump : end_bumps) {
            // Target: route begin_bump to end_bump?
            auto begin_tracks = interposer->available_tracks_bump_to_track(begin_bump);
            auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);
            for (auto& [t, _] : begin_tracks) {
                begin_tracks_vec.emplace_back(t);
            }
            
            auto path = this->route_path(interposer, begin_tracks_vec, this->track_map_to_track_set(end_tracks));

            std::Vector<hardware::Track*> temp_vec {};
            for (auto t: begin_tracks_vec){
                if (!begin_tracks.contains(t)){
                    temp_vec.emplace_back(t);
                }
            }
            begin_tracks_vec.swap(temp_vec);

            // Get begin and end track in path
            auto begin_track = path.front();
            auto end_track = path.back();

            // Connect the TOB from end track to end_bump
            if (!end_tracks.contains(end_track)){
                throw FinalError("MazeRouteStrategy::route_bump_to_bumps_net(): end track not in end tracks set");
            }

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
                begin_tracks_vec.emplace_back(t);
            } 

            total_length += (path_length(path) + 1);
        }

        return total_length + 1;
    }

    auto MazeRouteStrategy::route_track_to_bumps_net(hardware::Interposer* interposer, circuit::TrackToBumpsNet* net) const -> std::usize {
        debug::debug("Maze routing for track to bumps net");
        
        auto begin_track = net->begin_track();
        const auto& end_bumps = net->end_bumps();

        auto begin_tracks_vec = std::Vector<hardware::Track *>{begin_track};

        std::usize total_length {0};
        for (auto end_bump : end_bumps) {
            auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);
            auto path = this->route_path(interposer, begin_tracks_vec, this->track_map_to_track_set(end_tracks));

            // Get begin and end track in path
            auto begin_track = path.front();
            auto end_track = path.back();

            // Connect the TOB from end track to end_bump
            if (!end_tracks.contains(end_track)){
                throw FinalError("MazeRouteStrategy::route_track_to_bumps_net(): end track not in end tracks set");
            }
            end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
            end_tracks.find(end_track)->second.connect();

            // All track in path can see as `begin_track_set`
            for (auto t : path) {
                begin_tracks_vec.emplace_back(t);
            } 

            total_length += (path_length(path) + 1);
        }

        return total_length;
    }
    

    auto MazeRouteStrategy::route_bump_to_tracks_net(hardware::Interposer* interposer, circuit::BumpToTracksNet* net) const -> std::usize {
        debug::debug("Maze routing for bump to tracks net");

        auto begin_bump = net->begin_bump();
        const auto& end_tracks = net->end_tracks();

        auto begin_tracks_vec = std::Vector<hardware::Track*>{};

        std::usize total_length {0};
        for (auto end_track : end_tracks) {
            auto begin_tracks = interposer->available_tracks_bump_to_track(begin_bump);
            for (auto& [t, _] : begin_tracks) {
                begin_tracks_vec.emplace_back(t);
            }
    
            auto path = this->route_path(interposer, begin_tracks_vec, std::HashSet<hardware::Track *>{end_track});

            std::Vector<hardware::Track*> temp_vec {};
            for (auto t: begin_tracks_vec){
                if (!begin_tracks.contains(t)){
                    temp_vec.emplace_back(t);
                }
            }
            begin_tracks_vec.swap(temp_vec);

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
                begin_tracks_vec.emplace_back(t);
            } 

            total_length += path_length(path);
        }

        return total_length + 1;
    }

    auto MazeRouteStrategy::route_tracks_to_bumps_net(hardware::Interposer* interposer, circuit::TracksToBumpsNet* net) const -> std::usize {
        debug::debug("Maze routing for tracks to bumps net");
        
        auto& begin_tracks = net->begin_tracks();
        auto& end_bumps = net->end_bumps();

        auto begin_tracks_vec = std::Vector<hardware::Track*>{};
        for (auto t : begin_tracks) {
            begin_tracks_vec.emplace_back(t);
        }

        std::usize total_length {0};
        for (auto end_bump : end_bumps) {
            auto end_tracks = interposer->available_tracks_track_to_bump(end_bump);
            auto path = this->route_path(interposer, begin_tracks_vec, track_map_to_track_set(end_tracks));

            auto end_track = path.back();
            if (!end_tracks.contains(end_track)){
                throw FinalError("MazeRouteStrategy::route_tracks_to_bumps_net(): end track not in end tracks set");
            }

            for (auto t : path) {
                begin_tracks_vec.emplace_back(t);
            }

            total_length += (path_length(path) + 1);
        }

        return total_length;
    }

    auto MazeRouteStrategy::route_sync_net(hardware::Interposer* ptr_interposer, circuit::SyncNet* ptr_sync_net) const -> std::usize
    {
        try{
            // three: [bump_to_bump, track_to_bump, bump_to_track]

            debug::debug("Maze routing for synchronized nets");
            
            std::Array<std::Vector<routed_path>, 3> three_paths {};  
            std::Array<std::Vector<std::Option<hardware::Bump*>>, 3> three_end_bumps {};    
            std::Array<std::Vector<std::HashMap<hardware::Track*,\
                                                hardware::TOBConnector>>, 3> three_end_track_to_tob_maps {};    // connections between track and TOB
            std::HashSet<hardware::Track*> occupied_tracks_vec {}; 
            std::usize max_length {0};

            // set all begin/end tracks as occupied tracks
            // for these cannot be used by other nets
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

            // the first round of routing
            if (ptr_sync_net->btbnets().size() > 0){
                auto current_len = sync_preroute<circuit::BumpToBumpNet>(
                    ptr_interposer, ptr_sync_net->btbnets(),
                    three_paths.at(0), three_end_bumps.at(0), three_end_track_to_tob_maps.at(0),
                    occupied_tracks_vec
                );
                max_length = current_len > max_length ? current_len : max_length;
            }
            if (ptr_sync_net->ttbnets().size() > 0){
                auto current_len = sync_preroute<circuit::TrackToBumpNet>(
                    ptr_interposer, ptr_sync_net->ttbnets(),
                    three_paths.at(1), three_end_bumps.at(1), three_end_track_to_tob_maps.at(1),
                    occupied_tracks_vec
                );
                max_length = current_len > max_length ? current_len : max_length;
            }
            if (ptr_sync_net->bttnets().size() > 0){
                auto current_len = sync_preroute<circuit::BumpToTrackNet>(
                    ptr_interposer, ptr_sync_net->bttnets(),
                    three_paths.at(2), three_end_bumps.at(2), three_end_track_to_tob_maps.at(2),
                    occupied_tracks_vec
                );
                max_length = current_len > max_length ? current_len : max_length;
            }

            // reroute for adjusting length
            while (true){
                debug::debug("Route BumpToBump Synchronized Net");
                auto [success, ml] = sync_reroute(
                    ptr_interposer, three_paths.at(0), three_end_bumps.at(0), three_end_track_to_tob_maps.at(0),
                    2, max_length
                );
                if (success){
                    debug::debug("Route TrackToBump Synchronized Net");
                    auto [success, ml] = sync_reroute(
                        ptr_interposer, three_paths.at(1), three_end_bumps.at(1), three_end_track_to_tob_maps.at(1),
                        1, max_length
                    );
                    if (success){
                        debug::debug("Route BumpToTrack Synchronized Net");
                        auto [success, ml] = sync_reroute(
                            ptr_interposer, three_paths.at(2), three_end_bumps.at(2), three_end_track_to_tob_maps.at(2),
                            1, max_length
                        );
                        max_length = ml;
                        if (success){
                            break;      // break only when all three nets are successfully routed
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
//!
print_sync_path(ptr_sync_net);
//!
        std::usize total_nets {ptr_sync_net->btbnets().size() + ptr_sync_net->bttnets().size() + ptr_sync_net->ttbnets().size()};
        return total_nets * max_length; 
        }
        catch (const RetryExpt& e){
            throw e;
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
        const std::Vector<hardware::Track*>& begin_tracks,
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
                    if(prev_track_info == prev_track_infos.end()){
                        throw FinalError("MazeRouteStrategy::search_path(): cannot find previous track");
                    }
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

        throw RetryExpt("MazeRouteStrategy::search_path(): path not found");
    }

    auto MazeRouteStrategy::route_path(
        hardware::Interposer* interposer, 
        const std::Vector<hardware::Track*>& begin_tracks,
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
    catch (const RetryExpt& e){
        throw e;
    }
    catch (std::exception& e){
        throw std::runtime_error("MazeRouteStrategy::route_path() >> " + std::String(e.what()));
    }

    auto MazeRouteStrategy::track_map_to_track_vec(
        const std::HashMap<hardware::Track*, 
        hardware::TOBConnector>& map,
        const std::Array<std::usize, hardware::COB::UNIT_SIZE>& cobunit_usage
    ) const -> std::Vector<hardware::Track*> {
        auto vec = std::Vector<hardware::Track*>{};
        for (auto [t, _] : map) {
            vec.emplace_back(t);
        }
        std::sort(vec.begin(), vec.end(), [&cobunit_usage](hardware::Track* track1, hardware::Track* track2){
            auto cobunit = [](hardware::Track* track){
                auto index = track->coord().index;
                auto bank = index / (hardware::TOB::INDEX_SIZE / 2);
                return (bank*hardware::COBUnit::WILTON_SIZE + index%hardware::COBUnit::WILTON_SIZE);
            };
            auto cobunit1 = cobunit(track1);
            auto cobunit2 = cobunit(track2);
            return cobunit_usage[cobunit1] < cobunit_usage[cobunit2];
        });

        return vec;
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
            std::Vector<routed_path>& paths,
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
        if (sync_net.size() <= 0){
            throw FinalError("MazeRouteStrategy::sync_preroute(): empty sync_net");
        }

        std::Vector<hardware::Track*> begin_tracks_vec {};
        std::HashSet<hardware::Track*> end_tracks_set {};
        hardware::Bump* begin_bump = nullptr;
        hardware::Bump* end_bump = nullptr;
        std::HashMap<hardware::Track*, hardware::TOBConnector> begin_track_to_tob_map {};
        std::HashMap<hardware::Track*, hardware::TOBConnector> end_track_to_tob_map {};

        for (auto& uptr_net: sync_net){
            auto net = uptr_net.get();
            
            // collect begin bumps & begin tracks
            if constexpr (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::BumpToTrackNet>::value){
                begin_bump = net->begin_bump();
                begin_track_to_tob_map = interposer->available_tracks_bump_to_track(begin_bump);
                begin_tracks_vec = track_map_to_track_vec(begin_track_to_tob_map, begin_bump->tob()->cobunit_resources());
            }
            else if constexpr(std::is_same<Net, circuit::TrackToBumpNet>::value){
                begin_tracks_vec = std::Vector<hardware::Track*>{net->begin_track()};
            }
            // collect end bumps & end tracks
            if constexpr (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::TrackToBumpNet>::value){
                end_bump = net->end_bump();
                end_track_to_tob_map = interposer->available_tracks_track_to_bump(end_bump);
                end_tracks_set = track_map_to_track_set(end_track_to_tob_map);
            }
            else if constexpr (std::is_same<Net, circuit::BumpToTrackNet>::value){
                end_tracks_set = std::HashSet<hardware::Track*>{net->end_track()};
            }
            
            if (std::is_same<Net, circuit::BumpToBumpNet>::value){
                if (begin_bump->tob()->coord() == end_bump->tob()->coord()){
                    throw FinalError("MazeRouteStrategy::sync_preroute(): begin_bump_tob == end_bump_tob");
                }
            }

            // set end track of Net as unoccupied
            if constexpr(std::is_same<Net, circuit::BumpToTrackNet>::value){
                auto track = net->end_track();
                for (auto it = occupied_tracks_vec.begin(); it != occupied_tracks_vec.end(); ){
                    if ((*it)->coord() == track->coord()){
                        it = occupied_tracks_vec.erase(it);
                    }
                    else{
                        ++it;
                    }
                }
            }
            if constexpr(std::is_same<Net, circuit::TrackToBumpNet>::value){
                auto track = net->begin_track();
                for (auto it = occupied_tracks_vec.begin(); it != occupied_tracks_vec.end(); ){
                    if ((*it)->coord() == track->coord()){
                        it = occupied_tracks_vec.erase(it);
                    }
                    else{
                        ++it;
                    }
                }
            }

            // route and connect
            auto path_info = search_path(interposer, begin_tracks_vec, end_tracks_set, occupied_tracks_vec); // notice: negative sequence
                                                                                        //  |
            auto path = std::Vector<hardware::Track*>{};                                //  V
            hardware::Track* prev_track = nullptr;                                      // prev_track is the track before this
            for (auto iter = path_info.rbegin(); iter != path_info.rend(); ++iter) {    // get tracks in positive sequence        
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

            // connect begin bump / end bump
            if (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::BumpToTrackNet>::value){
                auto begin_track = path.front();
                if (!begin_track_to_tob_map.contains(begin_track)){
                    throw FinalError("MazeRouteStrategy::sync_preroute(): begin track not in begin track map");
                }
                begin_track_to_tob_map.find(begin_track)->second.connect();
                begin_bump->set_connected_track(begin_track, hardware::TOBSignalDirection::BumpToTrack);
            }
            if (std::is_same<Net, circuit::BumpToBumpNet>::value || std::is_same<Net, circuit::TrackToBumpNet>::value){
                auto end_track = path.back();
                if (!end_track_to_tob_map.contains(end_track)){
                    throw FinalError("MazeRouteStrategy::sync_preroute(): end track not in end track map");
                }
                end_track_to_tob_map.find(end_track)->second.connect();
                end_bump->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);

                end_bumps.emplace_back(end_bump);
                end_track_to_tob_maps.emplace_back(std::HashMap<hardware::Track*, hardware::TOBConnector>{*end_track_to_tob_map.find(end_track)});
            }

            routed_path reversed_path_info {};     // collect whole path in positive sequence
            std::transform(path_info.rbegin(), path_info.rend(), std::back_inserter(reversed_path_info), [](auto& pair) {
                return pair; 
            });
            paths.emplace_back(reversed_path_info);
        }
        
        // calculate length
        std::usize max_length = 0;
        for (auto& path: paths) {
            auto current_length = path_length(path);
            max_length = max_length < current_length ? current_length : max_length;
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
        std::Vector<routed_path>& paths,
        const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
        std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>& end_track_to_tob_maps,
        std::usize bump_extra_length, std::usize max_length
    ) const -> std::tuple<bool, std::usize>{
        std::Vector<routed_path*> nets_to_be_rerouted {};
        std::Vector<std::Option<hardware::Bump*>> related_end_bumps {};
        std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>*> related_maps {};

        // collect nets to be rerouted, along with their end bumps and track to tob maps
        for (std::usize i = 0; i < paths.size(); ++i) {
            auto& path = paths.at(i);
            if (path_length(path) + bump_extra_length < max_length) {
                nets_to_be_rerouted.emplace_back((&path));
                if (end_bumps.size() > 0){
                    related_end_bumps.emplace_back(end_bumps.at(i));
                }
                if (end_track_to_tob_maps.size() > 0){
                    related_maps.push_back(&end_track_to_tob_maps.at(i));
                }
            }
        }

        // reroute
        if (nets_to_be_rerouted.size() > 0) {
            auto [success, ml] = _rerouter->bus_reroute(
                interposer, nets_to_be_rerouted, max_length, related_end_bumps, related_maps, bump_extra_length
            );
            
            if (success){   // routing done with ml == max_length
                if (max_length != ml){
                    throw FinalError("MazeRouteStrategy::sync_reroute(): max_length != ml when succeed");
                }
                return std::tuple<std::usize, std::usize>{true, max_length};
            }
            else{           // have longer path OR routing failed
                return std::tuple<std::usize, std::usize>{false, ml};
            }
        }
        else{
            return std::tuple<std::usize, std::usize>{true, max_length};
        }
    }

    auto MazeRouteStrategy::print_sync_path(circuit::SyncNet* sync_net) const -> void {
        debug::debug("\nPrinting synchronized nets path...");
        auto& btbnets {sync_net->btbnets()};
        auto& bttnets {sync_net->bttnets()};
        auto& ttbnets {sync_net->ttbnets()};

        if (btbnets.size() > 0) {
            debug::debug("BumpToBump sync nets:");
            for (auto& net : btbnets) {
                auto begin_bump {net->begin_bump()};
                auto end_bump {net->end_bump()};
                auto path {begin_bump->connected_track()->track_path()};
                debug::debug_fmt("Begin_bump: ({}, index={})", begin_bump->coord(), begin_bump->index());
                for (auto& t: path){
                    debug::debug_fmt("{}", t->coord());
                }
                debug::debug_fmt("End_bump: ({}, index={})", end_bump->coord(), end_bump->index());
            }
            debug::debug("\n");
        }

        if (bttnets.size() > 0) {
            debug::debug("BumpToTrack sync nets:");
            for (auto& net : bttnets) {
                auto begin_bump {net->begin_bump()};
                auto path {begin_bump->connected_track()->track_path()};
                debug::debug_fmt("Begin_bump: ({}, index={})", begin_bump->coord(), begin_bump->index());
                for (auto& t: path){
                    debug::debug_fmt("{}", t->coord());
                }
            }
            debug::debug("\n");
        }

        if (ttbnets.size() > 0) {
            debug::debug("TrackToBump sync nets:");
            for (auto& net : ttbnets) {
                auto begin_track {net->begin_track()};
                auto end_bump {net->end_bump()};
                auto path {begin_track->track_path()};
                for (auto& t: path){
                    debug::debug_fmt("{}", t->coord());
                }
                debug::debug_fmt("End_bump: ({}, index={})", end_bump->coord(), end_bump->index());
            }
            debug::debug("\n");
        }
    }

    template<class InputNode, class OutputNode>
    auto MazeRouteStrategy::print_path(
        InputNode* input_node, OutputNode* output_node, const std::Vector<hardware::Track*>& path
    ) const -> void {
        debug::debug("\nPrinting path...");
        if constexpr (std::is_same<InputNode, hardware::Bump>::value){
            debug::debug_fmt("Begin_bump: ({}, index={})", input_node->coord(), input_node->index());
        }

        for (auto& t: path){
            debug::debug_fmt("{}", t->coord());
        }

        if constexpr (std::is_same<OutputNode, hardware::Bump>::value){
            debug::debug_fmt("End_bump: ({}, index={})", output_node->coord(), output_node->index());
        }
        debug::debug("\n");
    }
}