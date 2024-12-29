#include "./mazererouter.hh"
#include "../routeerror.hh"
#include <algorithm>
#include <format>
#include <cmath>
#include <stdexcept>
#include <format>

namespace kiwi::algo{

    auto Node::remove_child(const std::Rc<Node> child) -> void{
        auto it {std::find(_post_nodes.begin(), _post_nodes.end(), child)};
        if (it == _post_nodes.end()){
            auto coord {child->track()->coord()};
            throw FinalError(std::format("Node::remove_child: child at ({}, {}, {}, {}) not found", coord.row, coord.col, coord.dir, coord.index));
        }

        _post_nodes.erase(it);
    }

    auto Node::parent() -> std::Option<std::Rc<Node>>{
        if (this->_prev_node) {
            return std::make_optional<std::Rc<Node>>(this->_prev_node);
        } else {
            return std::nullopt;  
        }
    }

    Tree::Tree(const std::Rc<Node> root) : _root(root){
        if (root->parent() != std::nullopt || root->cost() != 0 || root->post_nodes().size() != 0){
            auto parent_coord {root->parent().value()->track()->coord()};
            kiwi::debug::fatal_fmt("Tree_constructor: root node must have null parent, empty children and a 0 cost, \
                                    but now it has parent ({}, {}, {}, {}), cost {} and {} children", \
                                    parent_coord.row, parent_coord.col, parent_coord.dir, parent_coord.index, \
                                    root->cost(), root->post_nodes().size());
        }
    }

    auto Tree::is_a_predecessor(const std::Rc<Node> current_node, const std::Rc<Node> to_be_checked) -> bool{
        assert(current_node != nullptr && to_be_checked != nullptr);
        if (*current_node == *to_be_checked){
            return true;
        }   
        else if (current_node->parent() == std::nullopt){
            return false;
        }
        else {
            return this->is_a_predecessor(current_node->parent().value(), to_be_checked);
        }
    }

    // positive sequence & do not contain the start node
    auto Tree::backtrace(const std::Rc<Node> current_node) -> std::vector<std::Rc<Node>>{

        std::Vector<std::Rc<Node>> path {};
        // end node at the head of path
        auto node = current_node;
        while (node->parent() != std::nullopt){
            path.push_back(node);
            node = node->parent().value();
        }

        // reverse
        std::reverse(path.begin(), path.end());
        return path;
    }

    auto MazeRerouter::bus_reroute(
            hardware::Interposer* interposer, std::Vector<routed_path*>& path_ptrs,
            std::usize max_length, const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>*>& end_track_to_tob_maps,
            std::usize bump_length
        ) const -> std::tuple<bool, std::usize>{

        for (std::usize i = 0; i < path_ptrs.size(); ++i){
            auto path_ptr {path_ptrs[i]};
            std::HashMap<hardware::Track*, hardware::TOBConnector>* end_track_to_tob_map {nullptr};
            if(end_track_to_tob_maps.size() > 0){
                end_track_to_tob_map = end_track_to_tob_maps[i];
            }

            std::HashSet<hardware::Track*> end_tracks_set {};
            std::HashMap<kiwi::hardware::Track *, kiwi::hardware::TOBConnector> possible_end_tracks_map{};

            // if there is an end bump, then the tob state will be updated after removing end track
            // --> possible end tracks will be changed
            if (end_track_to_tob_map != nullptr){
                remove_tracks(path_ptr, end_track_to_tob_map);

                // assert (path_ptrs.size() == end_bumps.size());
                if (path_ptrs.size() != end_bumps.size()){
                    throw FinalError("MazeRerouter::reroute: path vector is not end_bumps.size()");
                }
                auto end_bump {end_bumps[i]};
                // assert(end_bump.has_value());
                if (!end_bump.has_value()){
                    throw FinalError("MazeRerouter::reroute: end bump not found");
                }
                possible_end_tracks_map = interposer->available_tracks_track_to_bump(end_bump.value());
                for (auto& [track, _]: possible_end_tracks_map){
                    if (track && !end_tracks_set.contains(track)){
                        end_tracks_set.emplace(track);
                    }
                }
            }
            else{
                end_tracks_set.emplace(std::get<0>(path_ptr->back()));
                remove_tracks(path_ptr, end_track_to_tob_map);
            }

            // if failed, then routing failed  
            if (end_tracks_set.size() <= 0){
                throw FinalError("MazeRerouter::reroute: end_tracks_set is empty");
            }

            // construct a path-tree for reroute
            auto tree {Tree(_node_track_interface.track_rootify(std::get<0>(path_ptr->back()), std::get<1>(path_ptr->back()).value()))};
            auto [success, ml] = refind_path(interposer, tree, path_ptr, max_length, end_tracks_set, bump_length);
            max_length = ml;

            // connect end bump to end track
            if (end_track_to_tob_map != nullptr){
                auto end_track {std::get<0>(path_ptr->back())};
                auto end_bump {end_bumps[i]};
                if (!check_found(end_tracks_set, end_track)){
                    throw FinalError("MazeRerouter::reroute: end track not found");
                }
                
                // notice: the following "for" loop cannot be replaced by "possible_end_tracks_map.find(end_track)->second.connect();"
                // there will be unexpected behaviour during "find(end_track)"
                // for there are some other members not related to coordinates in value "end_track"
                // use "if (track->coord() == end_track->coord())" is safer
                for (auto& t: possible_end_tracks_map){
                    auto& [track, connector] = t;
                    if (track->coord() == end_track->coord()){
                        connector.connect();
                        *end_track_to_tob_maps[i] = std::HashMap<hardware::Track*, hardware::TOBConnector>{t};
                        break;
                    }
                }
                end_bump.value()->set_connected_track(end_track, hardware::TOBSignalDirection::TrackToBump);
            }

            if (!success){
                return {false, max_length};
            }
        }
        return {true, max_length};
    }

    auto MazeRerouter::remove_tracks(
        routed_path* path_ptr, std::HashMap<kiwi::hardware::Track *, kiwi::hardware::TOBConnector>* end_tracks, int cut_rate
    ) const -> void{
        //remove end TOBConnector
        if (end_tracks != nullptr){
            auto [end_track, connector] {path_ptr->back()};

            bool found = false;
            for (auto& t: *end_tracks){
                auto& [track, connector] = t;
                if (track->coord() == end_track->coord()){
                    found = true;
                    connector.disconnect();
                    break;
                }
            }

            auto end_bump {end_track->connected_bump()};
            if (!end_bump.has_value()){
                throw FinalError("MazeRerouter::remove_tracks: end bump not found");
            }
            (*end_bump)->disconnect_track(end_track);
        }
        
        auto path_length {path_ptr->size()};        // notice: do not use function path_length() here
        hardware::Track* next_track = nullptr;
        std::usize cut_length = path_length > 1 ? ((path_length * cut_rate) < 1 ? 1 : int(path_length * cut_rate)) : 0;
        std::usize remain_length {path_length - cut_length};
        for(std::usize i = remain_length; i < path_length; ++i){
            auto& pair = (*path_ptr)[i];
            auto& [track, connector] = pair;
            next_track = track->prev_track();
            if (next_track){
                track->disconnect_track(next_track);
            }
            if (connector.has_value()){
                connector.value().disconnect();
            }
        }
        path_ptr->resize(remain_length);
    }

    auto MazeRerouter::Manhattan_distance(const std::Rc<Node> node, const std::HashSet<hardware::Track*>& end_tracks) const -> std::usize{
        assert(end_tracks.size() > 0);
        auto co {(*end_tracks.begin())->coord()};
        for (auto it = end_tracks.begin(); it != end_tracks.end(); ++it){
            if((*it)->coord().row != co.row || (*it)->coord().col != co.col){
                throw std::runtime_error("Manhattan_distance: the end tracks are not in the same row or column");
            }
            co = (*it)->coord();
        }

        auto coord {(*end_tracks.begin())->coord()};
        return std::abs(node->track()->coord().row - coord.row) + std::abs(node->track()->coord().col - coord.col);
    }

    auto MazeRerouter::refind_path(
            hardware::Interposer* interposer, Tree& tree, routed_path* path_ptr,\
            std::usize max_length, const std::HashSet<hardware::Track*>& end_tracks, std::usize bump_length
        ) const -> std::tuple<bool, std::usize>{
        std::Vector<std::Rc<Node>> queue {tree._root};
        std::make_heap(queue.begin(), queue.end(), Node::CompareNodes);

//!
debug::debug("Rerouting...");
// print_end_tracks(end_tracks);
//!
        // mazing with A* 
        while (!queue.empty()) {
            // get current node
            std::pop_heap(queue.begin(), queue.end(), Node::CompareNodes);
            auto node_sptr {queue.back()};
            queue.pop_back();

            // find the end node
            auto track {node_sptr->track().get()};
            if (check_found(end_tracks, track)){                
                auto temp_node_list {tree.backtrace(node_sptr)};
                auto temp_path {_node_track_interface.nodes_trackify(temp_node_list)};
                auto temp_path_length {path_length(temp_path)};
                auto current_path_length {path_length(*path_ptr)};

                // find a longer path
                if (current_path_length + temp_path_length + bump_length > max_length){
                    // connect
                    hardware::Track* prev_track = nullptr;
                    for (auto it = temp_path.begin(); it != temp_path.end(); ++it){
                        auto& [track, connector] = *it;
                        if (prev_track != nullptr){
                            track->set_connected_track(prev_track);
                        }
                        if (connector.has_value()){
                            connector.value().connect();
                        }
                        prev_track = track;
                    }   
                    auto prev_path_tail {std::get<0>(path_ptr->back())};
                    auto temp_path_head {std::get<0>(temp_path.front())};
                    temp_path_head->set_connected_track(prev_path_tail);
                    // add to path
                    for (auto& tp: temp_path){
                        path_ptr->push_back(tp);
                    }
//!
// print_path(path_ptr);
//!
                    return {false, path_length(*path_ptr) + bump_length};
                }
                // find a equal path
                else if(current_path_length + temp_path_length + bump_length == max_length){
                    // connect
                    hardware::Track* prev_track = nullptr;
                    for (auto it = temp_path.begin(); it != temp_path.end(); ++it){
                        auto& [track, connector] = *it;
                        if (prev_track != nullptr){
                            track->set_connected_track(prev_track);
                        }
                        if (connector.has_value()){
                            connector.value().connect();
                        }
                        prev_track = track;
                    }
                    auto prev_path_tail {std::get<0>(path_ptr->back())};
                    auto temp_path_head {std::get<0>(temp_path.front())};
                    temp_path_head->set_connected_track(prev_path_tail);
                    // add to path
                    for (auto& tp: temp_path){
                        path_ptr->push_back(tp);
                    }
//!
// print_path(path_ptr);
//!
                    return {true, max_length};
                }
                // find a shorter path
                else{
                    auto parent {node_sptr->parent()};
                    if(parent.has_value()){
                        parent.value()->remove_child(node_sptr);
                    }
                }
            }

            // expand
            for (auto& [next_track, connector] : interposer->adjacent_idle_tracks(track)) {
                auto new_cost {node_sptr->cost() + 1 + Manhattan_distance(node_sptr, end_tracks)};
                auto next_track_sptr {std::make_shared<hardware::Track>(*next_track)};
                auto next_node {std::make_shared<Node>(next_track_sptr, connector, node_sptr, new_cost)};
                if (!tree.is_a_predecessor(node_sptr, next_node)){
                    node_sptr->add_child(next_node);
                    queue.push_back(next_node);
                    std::push_heap(queue.begin(), queue.end(), Node::CompareNodes);
                }
            }
        }

        throw RetryExpt("MazeRerouter::refind_path: path not found");
        return {false, max_length};
    }

    auto MazeRerouter::check_found(const std::HashSet<hardware::Track*>& end_tracks, hardware::Track* track) const -> bool {
        for (auto& end_track: end_tracks){
            if (track->coord() == end_track->coord()){
                return true;
            }
        }
        return false;
    }

    auto MazeRerouter::shared_cobs(const std::Vector<hardware::COBCoord>& cobs1, const std::Vector<hardware::COBCoord>& cobs2) const -> std::Vector<hardware::COBCoord>{
        std::Vector<hardware::COBCoord> shared_cobs {};
        for (auto& c1: cobs1){
            for (auto& c2: cobs2){
                if (c1 == c2){
                    shared_cobs.emplace_back(c1);
                }
            }
        }
        return shared_cobs;
    }

    // return all cobs connected with track
    auto MazeRerouter::track_pos_to_cobs(const hardware::Track* track) const -> std::Vector<hardware::COBCoord>{
        std::Vector<hardware::COBCoord> cobs {};
        auto coord {track->coord()};
        cobs.emplace_back(coord.row, coord.col);
        if (coord.dir == hardware::TrackDirection::Horizontal){
            if (coord.col >= 1){
                cobs.emplace_back(coord.row, coord.col - 1);
            }
        }
        else if (coord.dir == hardware::TrackDirection::Vertical){
            if (coord.row >= 1){
                cobs.emplace_back(coord.row - 1, coord.col);
            }
        }
        return cobs;
    }


    auto MazeRerouter::path_length(const routed_path& path, bool switch_length) const -> std::usize {
        assert(path.size() != 0);

        // path length = number of tracks
        if (switch_length){
            return path.size();
        }
        // for a group with number of consecutive tracks connected with the same COB >= 3, the length of the group is 2
        // because those tracks in the middle are not truely used as signal pathways
        else{
            std::usize head{0}, tail{0};
            std::usize path_length{0};

            if (path.size() <= 2){
                return path.size();
            }
            while(tail != path.size() - 1){
                auto current_pos {track_pos_to_cobs(std::get<0>(path[head]))};
                while (true){
                    auto tail_cobs {track_pos_to_cobs(std::get<0>(path[tail]))};
                    current_pos = shared_cobs(current_pos, tail_cobs);
                    if (current_pos.empty()){
                        path_length += 2;
                        head = tail;
                        break;
                    }
                    else{
                        if (tail == path.size() - 1){
                            break;
                        }
                        tail += 1;
                    }
                }
            }
            if (tail == head){
                path_length += 1;
            }
            else{
                path_length += 2;
            }

            return path_length;
        }
    }

    auto MazeRerouter::path_length(const std::Vector<hardware::Track*>& path, bool switch_length) const -> std::usize {
        assert(path.size() != 0);

        // path length = number of tracks
        if (switch_length){
            return path.size();
        }
        // for a group with number of consecutive tracks connected with the same COB >= 3, the length of the group is 2
        // because those tracks in the middle are not truely used as signal pathways
        else{
            std::usize head{0}, tail{0};
            std::usize path_length{0};

            if (path.size() <= 2){
                return path.size();
            }
            while(tail != path.size() - 1){
                auto current_pos {track_pos_to_cobs(path[head])};
                while (true){
                    auto tail_cobs {track_pos_to_cobs(path[tail])};
                    current_pos = shared_cobs(current_pos, tail_cobs);
                    if (current_pos.empty()){
                        path_length += 2;
                        head = tail;
                        break;
                    }
                    else{
                        if(tail == path.size() - 1){
                            break;
                        }
                        tail += 1;
                    }
                }
            }
            if (tail == head){
                path_length += 1;
            }
            else{
                path_length += 2;
            }

            return path_length;
        }
    }

}


