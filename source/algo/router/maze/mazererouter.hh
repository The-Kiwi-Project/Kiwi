#pragma once

#include <algo/router/reroutestrategy.hh>
#include <hardware/interposer.hh>
#include <std/collection.hh>
#include <std/integer.hh>
#include <std/memory.hh>
#include <std/utility.hh>
#include <global/debug/debug.hh>


namespace kiwi::hardware {
    class Interposer;
    class Track;
    class COBConnector;
}


namespace kiwi::algo{

    class Node{
        // A track with additional attributes
    public:
        Node(const std::Rc<hardware::Track> track, const hardware::COBConnector& connector, const std::Rc<Node> prev_node, std::usize cost)
        : _track{track}, _connector{connector}, _prev_node{prev_node}, _cost{cost}, _post_nodes{}
        {}
    
    public:
        auto remove_child(const std::Rc<Node> child) -> void;
        inline auto add_child(const std::Rc<Node> child) -> void {_post_nodes.emplace_back(child);}

        auto operator == (const Node& other) const -> bool {return this->_track->coord() == other._track->coord();}
        static inline bool CompareNodes(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
            return a->_cost > b->_cost; // min_heap
        }
    
    public:
        inline auto parent() -> std::Option<std::Rc<Node>>;
        inline auto track() -> std::Rc<hardware::Track> {return this->_track;}
        inline auto post_nodes() -> std::Vector<std::Rc<Node>>& {return this->_post_nodes;}
        inline auto cost() -> std::usize {return this->_cost;}
        inline auto connector() -> std::Option<hardware::COBConnector>& {return this->_connector;}

    private:
        std::Rc<hardware::Track> _track;
        std::Option<hardware::COBConnector> _connector;
        std::Rc<Node> _prev_node;
        std::Vector<std::Rc<Node>> _post_nodes; 
        std::usize _cost;
    };

    struct Tree{
        // A tree of nodes, describing the path
        Tree(const std::Rc<Node> root);

        auto is_a_predecessor(const std::Rc<Node> current_node, const std::Rc<Node> to_be_checked) -> bool;
        auto backtrace(const std::Rc<Node> node) -> std::Vector<std::Rc<Node>>;
    
        std::Rc<Node> _root;    
    };

    struct NodeTrackInterface{
        auto track_rootify(hardware::Track* track, hardware::COBConnector& connector) const -> std::Rc<Node>{
            auto track_sptr {std::make_shared<hardware::Track>(*track)};
            return std::make_shared<Node>(track_sptr, connector, nullptr, 0);
        }

        auto nodes_trackify(std::Vector<std::Rc<Node>>& nodes) const -> RerouteStrategy::routed_path{
            RerouteStrategy::routed_path tracks{};
            std::transform(nodes.begin(), nodes.end(), std::back_inserter(tracks), [](const std::Rc<Node>& node){
                hardware::Track* new_track = new hardware::Track(*node->track().get());
                std::Option<hardware::COBConnector>& connector {node->connector()};
                return std::tuple{new_track, connector};
            });
            return tracks;
        }
    };

    class MazeRerouter : public RerouteStrategy{
    public:
        auto reroute(       // reroute through pointer path_ptrs
            hardware::Interposer* interposer, std::Vector<routed_path*>& path_ptrs,
            std::usize max_length, const std::Vector<std::Option<hardware::Bump*>>& end_bumps,
            std::Vector<std::HashMap<hardware::Track*, hardware::TOBConnector>*>& end_track_to_tob_maps,
            std::usize bump_length
        ) const -> std::tuple<bool, std::usize> override;
    
    private:
        auto remove_tracks(
            routed_path* path_ptr, std::HashMap<kiwi::hardware::Track *, kiwi::hardware::TOBConnector>* end_tracks, int cut_rate = 0.2
        ) const -> void;
        auto reroute_single_net(
            hardware::Interposer* interposer, Tree& tree, routed_path* path_ptr,\
            std::usize max_length, const std::HashSet<hardware::Track*>& end_tracks, std::usize bump_length
        ) const -> std::tuple<bool, std::usize>;
        auto Manhattan_distance(const std::Rc<Node> node, const std::HashSet<hardware::Track*>& end_tracks) const -> std::usize;\
        auto check_found(const std::HashSet<hardware::Track*>& end_tracks, hardware::Track* track) const -> bool;
    
    private:
        const NodeTrackInterface _node_track_interface {};
    };

    
}


