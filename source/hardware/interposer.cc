#include "./interposer.hh"
#include "./cob/cob.hh"
#include "./tob/tob.hh"
#include "./track/track.hh"
#include "./bump/bump.hh"

#include <memory>
#include <ranges>
#include <std/collection.hh>
#include <std/utility.hh>
#include <std/integer.hh>
#include "hardware/cob/cobcoord.hh"
#include "hardware/coord.hh"
#include "hardware/track/trackcoord.hh"
#include "hardware/tob/tobcoord.hh"

#include <debug/debug.hh>

namespace kiwi::hardware {

    static auto build_tob_coord_map() -> std::HashMap<Coord, Coord> {
        auto map = std::HashMap<Coord, Coord>{};
        for (std::i64 row = 0; row < Interposer::TOB_ARRAY_HEIGHT; ++row) {
            for (std::i64 col = 0; col < Interposer::TOB_ARRAY_WIDTH; ++col) {
                auto coord = TOBCoord{row, col};
                auto coord_in_interposer = Coord{1 + 2 * row, 3 * col};
                map.emplace(coord, coord_in_interposer);
            }
        }
        return map;
    }

    const std::HashMap<Coord, Coord> Interposer::TOB_COORD_MAP = build_tob_coord_map();

    auto Interposer::is_external_port_coord(const TrackCoord& coord) -> bool {
        switch (coord.dir) {
            case hardware::TrackDirection::Horizontal: {
                return coord.col == 0 || coord.col == hardware::Interposer::COB_ARRAY_WIDTH;
            }
            case hardware::TrackDirection::Vertical: {
                return coord.row == 0 || coord.row == hardware::Interposer::COB_ARRAY_HEIGHT;
            }
        }

        debug::unreachable("Interposer::is_external_port_coord");
    }

    Interposer::Interposer() :
        _cobs{},
        _tobs{},
        _tracks{}
    {
        this->build();
    }

    void Interposer::clear() {
        this->_cobs.clear();
        this->_tobs.clear();
        this->_tracks.clear();
        this->build();
    }

    void Interposer::build() {
        for (std::i64 row = 0; row < Interposer::COB_ARRAY_HEIGHT; ++row) {
            for (std::i64 col = 0; col < Interposer::COB_ARRAY_WIDTH; ++col) {
                auto coord = COBCoord{row, col};
                this->_cobs.emplace(coord, std::make_unique<COB>(coord));
            }
        }

        for (std::i64 row = 0; row < Interposer::TOB_ARRAY_HEIGHT; ++row) {
            for (std::i64 col = 0; col < Interposer::TOB_ARRAY_WIDTH; ++col) {
                auto coord = TOBCoord{row, col};
                auto coord_in_interposer = Interposer::TOB_COORD_MAP.at(coord);
                this->_tobs.emplace(coord, std::make_unique<TOB>(coord, coord_in_interposer));
            }
        }
    }

    auto Interposer::available_tracks(Bump* bump, TOBSignalDirection dir) -> std::HashMap<Track*, TOBConnector> {
        auto tob = bump->tob();
        auto& coord = Interposer::TOB_COORD_MAP.at(tob->coord());

        auto accessable_cobunit = bump->accessable_cobunit();

        auto result = std::HashMap<Track*, TOBConnector>{};

        for (auto& connector : tob->available_connectors(bump->index(), dir)) {
            // check cobunit
            std::usize index = connector.track_index();
            std::usize bank_size = COB::INDEX_SIZE/2;
            std::usize wilton_size = COBUnit::WILTON_SIZE;
            std::usize cobunit_id = (index/bank_size)*wilton_size + (index%wilton_size);
            if (bump->accessable_cobunit().contains(cobunit_id)) {
                auto track = this->get_track(coord.row, coord.col, TrackDirection::Vertical, index);
                if (track.has_value()){
                    result.emplace(track.value(), connector);
                }
            }
        }
        return result;
    }  

    auto Interposer::available_tracks_bump_to_track(Bump* bump) -> std::HashMap<Track*, TOBConnector> {
        return this->available_tracks(bump, TOBSignalDirection::BumpToTrack);
    }

    auto Interposer::available_tracks_track_to_bump(Bump* bump) -> std::HashMap<Track*, TOBConnector> {
        return this->available_tracks(bump, TOBSignalDirection::TrackToBump);
    }

    auto Interposer::adjacent_idle_tracks(Track* track) -> std::Vector<std::Tuple<Track*, COBConnector>> {
        auto result = std::Vector<std::Tuple<Track*, COBConnector>>{};
        for (auto [t, connector] : this->adjacent_tracks(track)) {
            if (!connector.is_connected() && this->is_idle_tracks(t)) {
                result.emplace_back(t, connector);
            }
        }
        return result;
    }

    auto Interposer::adjacent_tracks(Track* track) -> std::Vector<std::Tuple<Track*, COBConnector>> {
        auto result = std::Vector<std::Tuple<Track*, COBConnector>>{};
        const auto& track_coord = track->coord();

        // For all cob coord around this track
        for (auto [from_dir, cob_coord] : track->adjacent_cob_coords()) {
            auto option_cob = this->get_cob(cob_coord);
            // Not a valid cob coord
            if (!option_cob.has_value()) {
                continue;
            }

            auto cob = *option_cob;
            
            // For each connector in cob's adjacent_connectors_from_right
            for (auto& connector : cob->adjacent_connectors(from_dir, track_coord.index)) {
                assert(connector.from_dir() == from_dir);

                auto new_track_coord = cob->to_dir_track_coord(connector.to_dir(), connector.to_track_index());

                // Get a track object from interposer
                // Track must exit, no need to check option
                auto track = this->get_track(new_track_coord).value();
                result.emplace_back(track, connector);
            }
        }

        return result;
    }

    auto Interposer::is_idle_tracks(Track* track) -> bool {
        for (auto [t, connector] : this->adjacent_tracks(track)) {
            if (connector.is_connected()) {
                return false;
            }
        }
        return true;
    }

    auto Interposer::get_cob(const COBCoord& coord) -> std::Option<COB*> {
        auto find_res = this->_cobs.find(coord);
        if (find_res == this->_cobs.end()) {
            return std::nullopt;
        } else {
            return { find_res->second.get() };
        }
    }

    auto Interposer::get_cob(std::i64 row, std::i64 col) -> std::Option<COB*> {
        return this->get_cob(COBCoord{row, col});
    }

    auto Interposer::get_tob(const TOBCoord& coord) -> std::Option<TOB*> {
        auto find_res = this->_tobs.find(coord);
        if (find_res == this->_tobs.end()) {
            return std::nullopt;
        } else {
            return { find_res->second.get() };
        }
    }

    auto Interposer::get_tob(std::i64 row, std::i64 col) -> std::Option<TOB*> {
        return this->get_tob(TOBCoord{row, col});
    }

    auto Interposer::get_track(const TrackCoord& coord) -> std::Option<Track*> {
        if (!Interposer::check_track_coord(coord)) {
            return std::nullopt;
        }
        return this->_tracks.emplace(coord, std::make_unique<Track>(coord)).first->second.get();
    }

    auto Interposer::get_track(std::i64 r, std::i64 c, TrackDirection d, std::usize i) -> std::Option<Track*> {
        return this->get_track(TrackCoord{r, c, d, i});
    }

    auto Interposer::get_bump(const TOBCoord& coord, std::usize index) -> std::Option<Bump*> {
        auto tob = this->get_tob(coord);
        if (!tob.has_value()) {
            return std::nullopt;
        }
        return (*tob)->get_bump(index);
    }

    auto Interposer::get_bump(std::i64 row, std::i64 col, std::usize index) -> std::Option<Bump*> {
        return this->get_bump(TOBCoord{row, col}, index);
    }

    auto Interposer::get_a_idle_tob() -> std::Option<TOB*> {
        for (auto& [coord, tob] : this->_tobs) {
            if (tob->is_idle()) {
                return {tob.get()};
            }
        }
        return std::nullopt;
    }

    auto Interposer::check_track_coord(const TrackCoord& coord) -> bool {
        return !( 
            coord.row < 0 || 
            coord.row > Interposer::COB_ARRAY_HEIGHT ||
            coord.col < 0 ||
            coord.col > Interposer::COB_ARRAY_WIDTH ||
            coord.index >= COB::INDEX_SIZE
        );
    }

    auto Interposer::manage_cobunit_resources() -> void {
        for (auto& [coord, tob]: this->_tobs) {
            tob->collect_cobunit_usage();
        }
    }

    auto Interposer::randomly_map_remain_indexes() -> void {
        for (auto& [_, tob] : this->_tobs) {
            tob->randomly_map_remain_indexes();
        }
    }

}