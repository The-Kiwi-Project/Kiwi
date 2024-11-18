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
            node = current_node->parent().value();
        }

        // reverse
        std::reverse(path.begin(), path.end());
        return path;
    }

    auto MazeRerouter::reroute(
            hardware::Interposer* interposer, std::Vector<routed_path*>& path_ptrs,
            std::usize max_length, const std::Vector<std::HashSet<hardware::Track*>>& end_tracks_set
        ) const -> std::tuple<bool, std::usize>{
        for (std::usize i = 0; i < path_ptrs.size(); ++i){
            auto path_ptr {path_ptrs[i]};
            auto end_tracks {end_tracks_set[i]};
            std::HashSet<hardware::Track*> visited_end_tracks {};
            for (std::usize j = 0; j < path_ptrs.size(); ++j)
            {
                if (i != j){
                    visited_end_tracks.insert(std::get<0>(path_ptrs[j]->back()));
                }
            }
            remove_tracks(path_ptr);                   
            auto tree {Tree(_node_track_interface.track_rootify(std::get<0>(path_ptr->back()), std::get<1>(path_ptr->back()).value()))};

            auto [success, ml] = reroute_single_net(interposer, tree, path_ptr, max_length, end_tracks, visited_end_tracks);
            max_length = ml;
            
            if (!success){
                return {false, max_length};
            }
        }
        return {true, max_length};
    }

    auto MazeRerouter::remove_tracks(routed_path* path_ptr, int rate) const -> void{
       auto path_length {path_ptr->size()};
       path_ptr->resize(path_length - std::round(path_length * rate));
    }

    auto MazeRerouter::Manhattan_distance(const std::Rc<Node> node, const std::HashSet<hardware::Track*>& end_tracks) const -> std::usize{
        for (auto it = end_tracks.begin(); it != end_tracks.end(); ++it){
            auto next_it {++it};
            if (next_it != end_tracks.end()){
                if(!((*it)->coord() == (*next_it)->coord())){
                    throw std::runtime_error("Manhattan_distance: the end tracks are not in the same row or column");
                }
            }
        }

        auto coord {(*end_tracks.begin())->coord()};
        return std::abs(node->track()->coord().row - coord.row) + std::abs(node->track()->coord().col - coord.col);
    }

    auto MazeRerouter::reroute_single_net(
            hardware::Interposer* interposer, Tree& tree, routed_path* path_ptr,\
            std::usize max_length, const std::HashSet<hardware::Track*>& end_tracks,
            std::HashSet<hardware::Track*>& visited_end_tracks
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
            auto found_end_track {end_tracks.find(track)};
            if (found_end_track != end_tracks.end()){
                // check if the end node is visited
                if (!visited_end_tracks.contains(*found_end_track)){
                    auto temp_node_list {tree.backtrace(node_sptr)};
                    auto temp_path {_node_track_interface.nodes_trackify(temp_node_list)};
                    if (path_ptr->size() + temp_path.size() > max_length){
                        for (auto tp: temp_path){
                            path_ptr->push_back(tp);
                        }
                        return {false, temp_path.size()};
                    }
                    else if(path_ptr->size() + temp_path.size() == max_length){
                        for (auto tp: temp_path){
                            path_ptr->push_back(tp);
                        }
                        return {true, max_length};
                    }
                    else{
                        auto parent {node_sptr->parent()};
                        assert(parent.has_value());
                        parent.value()->remove_child(node_sptr);
                    }
                }
                // the remove operation is optional and do not affect the result, only to keep tree & queue synchronized
                else{
                    auto parent {node_sptr->parent()};
                    assert(parent.has_value());
                    parent.value()->remove_child(node_sptr);
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

}


