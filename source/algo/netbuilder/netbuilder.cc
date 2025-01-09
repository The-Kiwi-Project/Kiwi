
#include "./netbuilder.hh"

#include <hardware/interposer.hh>
#include <circuit/net/nets.hh>
#include <circuit/basedie.hh>
#include <circuit/connection/connection.hh>
#include <circuit/connection/pin.hh>
#include <utility/string.hh>
#include <std/utility.hh>
#include <debug/debug.hh>

namespace kiwi::algo {

    void build_nets(circuit::BaseDie* basedie, hardware::Interposer* interposer) {
        algo::NetBuilder{basedie, interposer}.build();
    }

    NetBuilder::NetBuilder(circuit::BaseDie* basedie, hardware::Interposer* interposer) :
        _interposer{interposer},
        _basedie{basedie}
    {
    }

    auto NetBuilder::build() -> void
    try {
        for (auto& [sync, connections] : this->_basedie->connections()) {
            if (sync == -1) {
                this->build_no_sync_nets(connections);
            } else {
                this->build_sync_net(connections);
            }
        }

        this->build_fixed_nets();
    }
    THROW_UP_WITH("Build nets")

    auto NetBuilder::build_no_sync_nets(std::Span<const std::Box<circuit::Connection>> connections) -> void {
        // How to deal one to mul net?
        // Build a map: start to ends
        auto track_to_bumps = std::HashMap<hardware::Track*, std::Vector<hardware::Bump*>>{};
        auto bump_to_bumps = std::HashMap<hardware::Bump*, std::Vector<hardware::Bump*>>{};
        auto bump_to_tracks = std::HashMap<hardware::Bump*, std::Vector<hardware::Track*>>{};

        // Loop for each connection...
        for (auto& connection : connections) {
            auto& input = connection->input();
            auto& output = connection->output();

            if (NetBuilder::is_fixed_pin(output)) {
                debug::exception("Fixed pin as target??");
            }

            auto end_node = this->pin_to_node(output);

            if (NetBuilder::is_pose_pin(input)) {
                // Input is 'vdd'
                std::match(end_node, 
                    [&](hardware::Track* track) {
                        debug::exception_fmt("Invalid net with track to track: {} => {}", input, output);
                    },
                    [&](hardware::Bump* bump) {
                        this->_bumps_with_pose.emplace_back(bump);
                    }
                );
            } 
            else if (NetBuilder::is_nege_pin(input)) {
                // Input is 'gnd'
                std::match(end_node, 
                    [&](hardware::Track* track) {
                        debug::exception_fmt("Invalid net with track to track: {} => {}", input, output);
                    },
                    [&](hardware::Bump* bump) {
                        this->_bumps_with_nege.emplace_back(bump);
                    }
                );
            } 
            else {
                // Four case
                auto begin_node = this->pin_to_node(input);

                std::match(end_node, 
                    [&](hardware::Track* end_track) {
                        std::match(begin_node, 
                            [&](hardware::Track* begin_track) {
                                // 1. Track => Track
                                debug::exception_fmt("Invalid net with track to track: {} => {}", input, output);
                            },
                            [&](hardware::Bump* begin_bump) {
                                // 2. Bump => Track
                                auto& end_tracks = bump_to_tracks.emplace(begin_bump, std::Vector<hardware::Track*>{}).first->second;
                                end_tracks.emplace_back(end_track);
                            }
                        );
                    },
                    [&](hardware::Bump* end_bump) {
                        std::match(begin_node, 
                            [&](hardware::Track* begin_track) {
                                // 3. Track => Bump
                                auto& end_bumps = track_to_bumps.emplace(begin_track, std::Vector<hardware::Bump*>{}).first->second;
                                end_bumps.emplace_back(end_bump);  
                            },
                            [&](hardware::Bump* begin_bump) {
                                // 4. Bump => Bump
                                auto& end_bumps = bump_to_bumps.emplace(begin_bump, std::Vector<hardware::Bump*>{}).first->second;
                                end_bumps.emplace_back(end_bump);
                            }
                        );
                    }
                );
            }
        }

        // Track => Bump
        for (auto& [begin_track, end_bumps] : track_to_bumps) {
            
            // Create net 
            auto net = std::Box<circuit::Net>{};
            if (end_bumps.size() == 1) {
                net = std::make_unique<circuit::TrackToBumpNet>(begin_track, end_bumps[0]);
            } else {
                net = std::make_unique<circuit::TrackToBumpsNet>(begin_track, std::move(end_bumps));
            }

            // Insert to basedie & topdieinsts
            auto topdie_insts = std::HashSet<circuit::TopDieInstance*>{};
            for (auto bump : end_bumps) {
                topdie_insts.emplace(this->_bump_to_topdie_inst.at(bump));
            }

            for (auto inst : topdie_insts) {
                inst->add_net(net.get());
            }

            this->_basedie->add_net(std::move(net));
        }

        // Bump => Bump
        for (auto& [begin_bump, end_bumps] : bump_to_bumps) {
            // Create net 
            auto net = std::Box<circuit::Net>{};
            if (end_bumps.size() == 1) {
                net = std::make_unique<circuit::BumpToBumpNet>(begin_bump, end_bumps.front());
            } else {
                net = std::make_unique<circuit::BumpToBumpsNet>(begin_bump, std::move(end_bumps));
            }

            // Insert to basedie & topdieinsts
            auto topdie_insts = std::HashSet<circuit::TopDieInstance*>{ };
            topdie_insts.emplace(this->_bump_to_topdie_inst.at(begin_bump));
            for (auto bump : end_bumps) {
                topdie_insts.emplace(this->_bump_to_topdie_inst.at(bump));
            }

            for (auto inst : topdie_insts) {
                inst->add_net(net.get());
            }

            this->_basedie->add_net(std::move(net));
        }

        // Bump => Track
        for (auto& [begin_bump, end_tracks] : bump_to_tracks) {
            // Create net 
            auto net = std::Box<circuit::Net>{};
            if (end_tracks.size() == 1) {
                net = std::make_unique<circuit::BumpToTrackNet>(begin_bump, end_tracks.front());
            } else {
                net = std::make_unique<circuit::BumpToTracksNet>(begin_bump, std::move(end_tracks));
            }

            // Insert to basedie & topdieinsts
            this->_bump_to_topdie_inst.at(begin_bump)->add_net(net.get());
            this->_basedie->add_net(std::move(net));
        }
    }

    auto NetBuilder::build_sync_net(std::Span<const std::Box<circuit::Connection>> connections) -> void {
        auto btb_sync_nets = std::Vector<std::Box<circuit::BumpToBumpNet>>{};
        auto btt_sync_nets = std::Vector<std::Box<circuit::BumpToTrackNet>>{};
        auto ttb_sync_nets = std::Vector<std::Box<circuit::TrackToBumpNet>>{};

        for (auto& connection : connections) {
            auto& input = connection->input();
            auto& output = connection->output();

            if (NetBuilder::is_fixed_pin(input) || NetBuilder::is_fixed_pin(output)) {
                debug::exception("Fixed pin can't as sync");
            }

            auto begin_node  = this->pin_to_node(input);
            auto end_node = this->pin_to_node(output);
        
            // Each connec in sync net should has the same TOB size. 
            // We do not check this condition.... :)
            // Case four case
            std::match(end_node, 
                [&](hardware::Track* end_track) {
                    std::match(begin_node, 
                        [&](hardware::Track* begin_track) {
                            // 1. Track => Track
                            debug::exception_fmt("Invalid net with track to track: {} => {}", input, output);
                        },
                        [&](hardware::Bump* begin_bump) {
                            // 2. Bump => Track
                            auto net = std::make_unique<circuit::BumpToTrackNet>(begin_bump, end_track);
                            this->_bump_to_topdie_inst.at(begin_bump)->add_net(net.get());
                            btt_sync_nets.emplace_back(std::move(net));
                        }
                    );
                },
                [&](hardware::Bump* end_bump) {
                    std::match(begin_node, 
                        [&](hardware::Track* begin_track) {
                            // 3. Track => Bump
                            auto net = std::make_unique<circuit::TrackToBumpNet>(begin_track, end_bump);
                            this->_bump_to_topdie_inst.at(end_bump)->add_net(net.get());
                            ttb_sync_nets.emplace_back(std::move(net)); 
                        },
                        [&](hardware::Bump* begin_bump) {
                            // 4. Bump to Bump
                            auto net = std::make_unique<circuit::BumpToBumpNet>(begin_bump, end_bump);
                            this->_bump_to_topdie_inst.at(begin_bump)->add_net(net.get());
                            this->_bump_to_topdie_inst.at(end_bump)->add_net(net.get());
                            btb_sync_nets.emplace_back(std::move(net)); 
                        }
                    );
                }
            );
        }

        this->_basedie->add_net(std::make_unique<circuit::SyncNet>(
            std::move(btb_sync_nets),
            std::move(btt_sync_nets),
            std::move(ttb_sync_nets)
        ));
    }

    auto NetBuilder::build_fixed_nets() -> void {
        // Add nege
        if (!this->_bumps_with_nege.empty()) {
            auto nege_tracks = std::Vector<hardware::Track*>{};
            for (auto& track_coord : NetBuilder::_nege_tracks) {
                auto track = this->_interposer->get_track(track_coord);
                if (!track.has_value()) {
                    debug::exception_fmt("Invalid track coord '{}'", track_coord);
                } else {
                    nege_tracks.emplace_back(*track);
                }
            }

            this->_basedie->add_net(std::make_unique<circuit::TracksToBumpsNet>(
                std::move(nege_tracks), std::move(this->_bumps_with_nege)
            ));
        }

        // Add pose
        if (!this->_bumps_with_pose.empty()) {
            auto pose_tracks = std::Vector<hardware::Track*>{};
            for (auto& track_coord : NetBuilder::_pose_tracks) {
                auto track = this->_interposer->get_track(track_coord);
                if (!track.has_value()) {
                    debug::exception_fmt("Invalid track coord '{}'", track_coord);
                } else {
                    pose_tracks.emplace_back(*track);
                }
            }

            this->_basedie->add_net(std::make_unique<circuit::TracksToBumpsNet>(
                std::move(pose_tracks), std::move(this->_bumps_with_pose)
            ));
        }
    }

    auto NetBuilder::pin_to_node(const circuit::Pin& pin) -> Node {
        return std::match(pin.connected_point(),
            [this](const circuit::ConnectExPort& eport) {
                auto& external_port = eport.port;
                auto track = this->_interposer->get_track(external_port->coord());
                if (!track.has_value()) {
                    debug::exception_fmt("External port '{}' has an invalid track coord", external_port->name(), external_port->coord());
                } else {
                    return Node{*track};
                }
            },
            [this](const circuit::ConnectBump& connect_bump) {
                /// 
                /// Topdie inst: Search inst by name, and get bump index, and get bump object in interposer
                ///
                // Is pin exit?
                auto topdie = connect_bump.inst->topdie();
                auto res = topdie->pins_map().find(connect_bump.name);
                if (res == topdie->pins_map().end()) {
                    debug::exception_fmt("No exit pin name '{}' in topdie '{}'", connect_bump.name, topdie->name());
                }

                auto coord = connect_bump.inst->tob()->coord();
                auto index = res->second;

                auto bump = this->_interposer->get_bump(coord, index);
                if (!bump.has_value()) {
                    debug::exception_fmt("Pin '{}' has an invalid bump coord: TOBCoord '{}' with '{}'", connect_bump.name, coord, index);
                }
                this->_bump_to_topdie_inst.emplace(*bump, connect_bump.inst);
                return Node{*bump};
            }
        );
    }

    auto NetBuilder::is_pose_pin(const circuit::Pin& pin) -> bool {
        return std::match(pin.connected_point(),
            [](const circuit::ConnectExPort& eport) {
                return eport.port->name().ends_with("pose");
            },
            [](const circuit::ConnectBump&) {
                return false;
            }
        );
    }

    auto NetBuilder::is_nege_pin(const circuit::Pin& pin) -> bool {
        return std::match(pin.connected_point(),
            [](const circuit::ConnectExPort& eport) {
                return eport.port->name().ends_with("nege");
            },
            [](const circuit::ConnectBump&) {
                return false;
            }
        );
    }

    auto NetBuilder::is_fixed_pin(const circuit::Pin& pin) -> bool {
        return NetBuilder::is_pose_pin(pin) || NetBuilder::is_nege_pin(pin);
    }

    std::Vector<hardware::TrackCoord> NetBuilder::_pose_tracks {
        // group1
        hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 10}, hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 19},
        hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 28}, hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 37},
        // group2
        hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 46}, hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 55},
        hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 122}, hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 115},
        // group3
        hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 108}, hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 101},
        hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 94}, hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 87}
    };
        
    std::Vector<hardware::TrackCoord> NetBuilder::_nege_tracks {
        // group1
        hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 17}, hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 26},
        hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 35}, hardware::TrackCoord{0, 5, hardware::TrackDirection::Vertical, 44},
        // group2
        hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 53}, hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 62},
        hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 98}, hardware::TrackCoord{9, 4, hardware::TrackDirection::Vertical, 105},
        // group3
        hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 91}, hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 84},
        hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 77}, hardware::TrackCoord{4, 12, hardware::TrackDirection::Horizontal, 70}
    };

}