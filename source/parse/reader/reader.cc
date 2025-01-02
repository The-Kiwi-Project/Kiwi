/*

    1. Load config

    2. Create interposer

    3. Create basedie

    4. Add topdie to basedie (_name, _pins_map)

    5. Add topdieinst to basedie (_name, _topdie, _tob, _nets is empty now)

    6. Create Net

        for each sync in config.connections

        - if sync == -1 => create net to basedie
        - else, all net has the same sync will see as one sync net
            so, create sync, and add each net into it

        > basedie owns nets, topdieinst borrows nets 

*/

#include "./reader.hh"

#include "./config/config.hh"
#include "circuit/net/types/bbnet.hh"
#include "circuit/net/types/btnet.hh"
#include "circuit/net/types/tbnet.hh"
#include "hardware/bump/bump.hh"
#include "hardware/track/track.hh"
#include "std/collection.hh"
#include "std/string.hh"
#include "std/utility.hh"
#include <std/range.hh>
#include <std/memory.hh>

#include <hardware/interposer.hh>
#include <hardware/tob/tob.hh>
#include <circuit/basedie.hh>
#include <circuit/topdie/topdie.hh>
#include <circuit/topdie/topdieinst.hh>
#include <circuit/net/nets.hh>

#include <debug/debug.hh>
#include <utility/string.hh>

namespace kiwi::parse {

    Reader::Reader(const Config& config) :
        _config{config},
        _interposer{},
        _basedie{}
    {
        this->_interposer = this->create_interposer();
        this->_basedie = this->create_basedir();
    }

    auto Reader::build() -> std::Tuple<std::Box<hardware::Interposer>, std::Box<circuit::BaseDie>>
    try {
        this->add_topdies_to_basedie();
        this->add_topdieinst_to_basedie();
        this->add_nets();
        for (auto& n : this->_basedie->nets()) {
            // debug::debug_fmt("{}", n->to_string());
        }
        return { std::move(this->_interposer), std::move(this->_basedie) };
    }
    THROW_UP_WITH("Build system")

    auto Reader::create_interposer() -> std::Box<hardware::Interposer> {
        return std::make_unique<hardware::Interposer>();
    }

    auto Reader::create_basedir() -> std::Box<circuit::BaseDie> {
        return std::make_unique<circuit::BaseDie>();
    }

    auto Reader::add_topdies_to_basedie() -> void 
    try {
        auto& topdies_config = this->_config.topdies;

        for (auto& [name, config] : topdies_config) {
            this->_basedie->add_topdie(name, config.pin_map);
        }
    }
    THROW_UP_WITH("Add topdies to baesedie")

    auto Reader::add_topdieinst_to_basedie() -> void 
    try {
        auto& topdie_insts = this->_config.topdie_insts;

        for (auto& [inst_name, config] : topdie_insts) {
            auto& topdie_name = config.topdie;
            auto& coord = config.coord;

            // Get tob from interposer
            auto tob = this->_interposer->get_tob(coord);
            if (!tob.has_value()) {
                debug::exception_fmt("Invald coord '{}'", coord);
            }

            // Get topdie from baesdie
            auto res = this->_basedie->topdies().find(topdie_name);
            if (res == this->_basedie->topdies().end()) {
                debug::exception_fmt("No exit topdie '{}'", topdie_name);
            }
            auto& topdie = res->second;

            // Add into basedie
            this->_basedie->add_topdie_inst(inst_name, topdie.get(), *tob);
        }
    }
    THROW_UP_WITH("Add topdies inst to baesedie")

    auto Reader::add_nets() -> void 
    try {
        build_01_ports(this->_config.ports_01);
        for (auto& [sync, connections] : this->_config.connections) {
            if (sync == -1) {
                this->build_no_sync_nets(connections);
            } else {
                this->build_sync_net(connections);
            }
        }

        this->build_fixed_nets();
    }
    THROW_UP_WITH("Add net")

    auto Reader::build_no_sync_nets(std::Span<const ConnectionConfig> connections) -> void {
        // How to deal one to mul net?
        // Build a map: start to ends
        auto track_to_bumps = std::HashMap<hardware::Track*, std::Vector<hardware::Bump*>>{};
        auto bump_to_bumps = std::HashMap<hardware::Bump*, std::Vector<hardware::Bump*>>{};
        auto bump_to_tracks = std::HashMap<hardware::Bump*, std::Vector<hardware::Track*>>{};

        // Loop for each connection...
        for (auto& connection : connections) {
            auto& input = connection.input;
            auto& output = connection.output;

            if (Reader::is_fixed_pin(output)) {
                debug::exception("Fixed pin as target??");
            }

            auto end_node = this->parse_connection_pin(output);

            if (Reader::is_pose_pin(input)) {
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
            else if (Reader::is_nege_pin(input)) {
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
                auto begin_node = this->parse_connection_pin(input);

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
                net = std::make_unique<circuit::TrackToBumpNet>(begin_track, end_bumps.at(0));
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

    auto Reader::build_sync_net(std::Span<const ConnectionConfig> connections) -> void {
        auto btb_sync_nets = std::Vector<std::Box<circuit::BumpToBumpNet>>{};
        auto btt_sync_nets = std::Vector<std::Box<circuit::BumpToTrackNet>>{};
        auto ttb_sync_nets = std::Vector<std::Box<circuit::TrackToBumpNet>>{};

        for (auto& connection : connections) {
            auto& input = connection.input;
            auto& output = connection.output;

            if (Reader::is_fixed_pin(input) || Reader::is_fixed_pin(output)) {
                debug::exception("Fixed pin can't as sync");
            }

            auto begin_node  = this->parse_connection_pin(input);
            auto end_node = this->parse_connection_pin(output);
        
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

    auto Reader::build_fixed_nets() -> void {
        // Add nege
        if (!this->_bumps_with_nege.empty()) {
            auto nege_tracks = std::Vector<hardware::Track*>{};
            for (auto& track_coord : Reader::_nege_tracks) {
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
            for (auto& track_coord : Reader::_pose_tracks) {
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

    auto Reader::build_01_ports(const auto& ports_config) -> void {
        if (!ports_config.contains("pose") || !ports_config.contains("nege")) {
            debug::exception("No pose or nege ports");
        }
        this->_pose_tracks = parse_01(ports_config.at("pose"));
        this->_nege_tracks = parse_01(ports_config.at("nege"));
    }

    auto Reader::parse_connection_pin(std::StringView name) -> Reader::Node {
        if (std::StringView::npos == name.find('.')) {
            /// 
            /// External port: Search external ports in config, and get track object in interposer
            ///
            auto res = this->_config.external_ports.find(std::String{name});
            if (res == this->_config.external_ports.end()) {
                debug::exception_fmt("No exit expoert '{}'", name);
            }

            auto& external_port = res->second;
            auto track = this->_interposer->get_track(external_port.coord);
            if (!track.has_value()) {
                debug::exception_fmt("External port '{}' has an invalid track coord", name, external_port.coord);
            } else {
                return *track;
            }
        } 
        else {
            /// 
            /// Topdie inst: Search inst by name, and get bump index, and get bump object in interposer
            ///
            auto tokens = utility::split(name, '.');
            if (tokens.size() != 2) {
                debug::exception_fmt("Invalid topinst' pin name '{}'", name);
            }
            auto topdie_inst_name = tokens.at(0);
            auto pin_name = tokens.at(1);

            // Is inst exit?
            auto inst = this->_basedie->get_topdie_inst(topdie_inst_name);
            if (!inst.has_value()) {
                debug::exception_fmt("No exit topdie insta '{}'", topdie_inst_name);
            }

            // Is pin exit?
            auto topdie = (*inst)->topdie();
            auto res = topdie->pins_map().find(std::String{pin_name});
            if (res == topdie->pins_map().end()) {
                debug::exception_fmt("No exit pin name '{}' in topdie '{}'", pin_name, topdie->name());
            }

            auto coord = (*inst)->tob()->coord();
            auto index = res->second;

            auto bump = this->_interposer->get_bump(coord, index);
            if (!bump.has_value()) {
                debug::exception_fmt("Pin '{}' has an invalid bump coord: TOBCoord '{}' with '{}'", name, coord, index);
            }
            this->_bump_to_topdie_inst.emplace(*bump, *inst);
            return *bump;
        }
    }

    auto Reader::parse_01(const std::HashMap<std::String, hardware::TrackCoord>& ports) -> std::Vector<hardware::TrackCoord> {
        auto tracks = std::Vector<hardware::TrackCoord>{};
        for (auto& [_, coord] : ports) {
            tracks.emplace_back(coord);
        }
        return std::move(tracks);
    }

    auto Reader::is_pose_pin(std::StringView name) -> bool {
        return name.ends_with("pose");
    }

    auto Reader::is_nege_pin(std::StringView name) -> bool {
        return name.ends_with("nege");
    }

    auto Reader::is_fixed_pin(std::StringView name) -> bool {
        return Reader::is_pose_pin(name) || Reader::is_nege_pin(name);
    }

}