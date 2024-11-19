#include "./mazererouter.hh"
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
            throw std::runtime_error(std::format("Node::remove_child: child at ({}, {}, {}, {}) not found", coord.row, coord.col, coord.dir, coord.index));
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
        if (current_node == to_be_checked){
            return true;
        }   
        else if (current_node->parent() == std::nullopt){
            return false;
        }
        else {
            return this->is_a_predecessor(current_node->parent().value(), to_be_checked);
        }
    }

    // path tail at the end & do not contain the start node
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

    auto MazeRerouter::reroute(
            hardware::Interposer* interposer, std::Vector<routed_path*>& path_ptrs,
            std::usize max_length, const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>>& end_track_to_tob_maps,
            std::usize bump_length
        ) const -> std::tuple<bool, std::usize>{

        for (std::usize i = 0; i < path_ptrs.size(); ++i){
            auto path_ptr {path_ptrs[i]};
            std::HashMap<hardware::Track*, hardware::TOBConnector>* end_track_to_tob_map {nullptr};
            if(end_track_to_tob_maps.size() > 0){
                end_track_to_tob_map = &end_track_to_tob_maps[i];
            }

            std::HashSet<hardware::Track*> end_tracks_set {};
            std::HashMap<kiwi::hardware::Track *, kiwi::hardware::TOBConnector> possible_end_tracks_map{};

            remove_tracks(path_ptr, end_track_to_tob_map); 

            if (end_track_to_tob_map != nullptr){
                assert (path_ptrs.size() == end_bumps.size());
                auto end_bump {end_bumps[i]};
                assert(end_bump.has_value());

                //! 这里的emplace顺序和end_tracks_map当中track*指向的track在内存的顺序不一致导致报错 transposed pointer range
                //! 发现当remove_tracks里面的disconnect()的bump->hori部分会对上述过程引入错误，把这个注释掉就没错误了
                possible_end_tracks_map = interposer->available_tracks_track_to_bump(end_bump.value());
                for (auto& [track, connector]: possible_end_tracks_map){
                    auto coord {track->coord()};
                }
                //! 上面这段别删
                for (auto& [track, _]: possible_end_tracks_map){
                    if (track && !end_tracks_set.contains(track)){
                        end_tracks_set.emplace(track);
                    }
                }
            }
            else{
                end_tracks_set.emplace(std::get<0>(path_ptr->back()));
            }
 
            assert(end_tracks_set.size() > 0);                 
            auto tree {Tree(_node_track_interface.track_rootify(std::get<0>(path_ptr->back()), std::get<1>(path_ptr->back()).value()))};
    
            auto [success, ml] = reroute_single_net(interposer, tree, path_ptr, max_length, end_tracks_set, bump_length);
            max_length = ml;

            if (end_track_to_tob_map != nullptr){
                auto end_track {std::get<0>(path_ptr->back())};
                auto end_bump {end_bumps[i]};
                assert(check_found(end_tracks_set, end_track));
                
                for (auto& t: possible_end_tracks_map){
                    auto& [track, connector] = t;
                    if (track->coord() == end_track->coord()){
                        connector.connect();
                        end_track_to_tob_maps[i] = std::HashMap<hardware::Track*, hardware::TOBConnector>{t};
                        break;
                    }
                }
                // 这里的find在哈希的时候有点bug，虽然坐标一样但是_prev_track_和_next_track_不一样
                // possible_end_tracks_map.find(end_track)->second.connect();
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
            assert (end_bump.has_value());
            (*end_bump)->disconnect_track(end_track);
        }
        
        auto path_length {path_ptr->size()};
        hardware::Track* next_track = nullptr;
        std::usize remain_length {path_length - int(path_length * cut_rate)};
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

    auto MazeRerouter::reroute_single_net(
            hardware::Interposer* interposer, Tree& tree, routed_path* path_ptr,\
            std::usize max_length, const std::HashSet<hardware::Track*>& end_tracks, std::usize bump_length
        ) const -> std::tuple<bool, std::usize>{
        std::Vector<std::Rc<Node>> queue {tree._root};
        std::make_heap(queue.begin(), queue.end(), Node::CompareNodes);

        while (!queue.empty()) {
            // get current node
            std::pop_heap(queue.begin(), queue.end(), Node::CompareNodes);
            auto node_sptr {queue.back()};
            queue.pop_back();

            // find the end node
            auto track {node_sptr->track().get()};
            if (check_found(end_tracks, track)){                // 这里直接用contains可能哈希过程有问题，因为track*里面不只有坐标，还有其他值
                auto temp_node_list {tree.backtrace(node_sptr)};
                auto temp_path {_node_track_interface.nodes_trackify(temp_node_list)};

                if (path_ptr->size() + temp_path.size() + bump_length > max_length){
                    hardware::Track* next_track = nullptr;
                    for (auto rit = temp_path.rbegin(); rit != temp_path.rend(); ++rit){
                        auto& [track, connector] = *rit;
                        if (next_track){
                            track->set_connected_track(next_track);
                        }
                        if (connector.has_value()){
                            connector.value().connect();
                        }
                        next_track = track;
                    }
                    for (auto& tp: temp_path){
                        path_ptr->push_back(tp);
                    }
                    return {false, temp_path.size()};
                }
                else if(path_ptr->size() + temp_path.size() + bump_length == max_length){
                    hardware::Track* next_track = nullptr;
                    for (auto rit = temp_path.rbegin(); rit != temp_path.rend(); ++rit){
                        auto& [track, connector] = *rit;
                        if (next_track){
                            track->set_connected_track(next_track);
                        }
                        if (connector.has_value()){
                            connector.value().connect();
                        }
                        next_track = track;
                    }
                    for (auto& tp: temp_path){
                        path_ptr->push_back(tp);
                    }
                    return {true, max_length};
                }
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

        debug::error("MazeRerouter::reroute_single_net: no path found");
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

}


