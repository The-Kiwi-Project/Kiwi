#include "./tob.hh"
#include "../coord.hh"
#include "../bump/bump.hh"
#include "../tob/tobcoord.hh"
#include "../tob/tobregister.hh"
#include "hardware/bump/bumpcoord.hh"

#include <std/utility.hh>
#include <std/collection.hh>
#include <debug/debug.hh>
#include <std/range.hh>
#include <std/utility.hh>
#include <cassert>

namespace kiwi::hardware {

    TOB::TOB(TOBCoord const& coord, Coord const& coord_in_interposer) :
        _coord{coord},
        _coord_in_interposer(coord_in_interposer)
    {
        for (auto i : std::ranges::views::iota(0, (int)TOB::BUMP_TO_HORI_MUX_COUNT)) {
            this->_bump_to_hori_muxs.emplace_back(std::make_unique<TOBMux>(TOB::BUMP_TO_HORI_MUX_SIZE));
        }

        for (auto i : std::ranges::views::iota(0, (int)TOB::HORI_TO_VERI_MUX_COUNT)) {
            this->_hori_to_vert_muxs.emplace_back(std::make_unique<TOBMux>(TOB::HORI_TO_VERI_MUX_SIZE));
        }

        for (auto i : std::ranges::views::iota(0, (int)TOB::VERI_TO_TRACK_MUX_COUNT)) {
            this->_vert_to_track_muxs.emplace_back(std::make_unique<TOBMux>(TOB::VERI_TO_TRACK_MUX_SIZE));
        }

        for (auto i : std::ranges::views::iota(0, (int)TOB::INDEX_SIZE)) {
            this->_bump_dir_registers.emplace_back(std::make_unique<TOBBumpDirRegister>());
        }

        for (auto i : std::ranges::views::iota(0, (int)TOB::INDEX_SIZE)) {
            this->_track_dir_registers.emplace_back(std::make_unique<TOBTrackDirRegister>());
        }
    }

    TOB::TOB(std::i64 row, std::i64 col) :
        TOB{TOBCoord{row, col}, Coord{row, col}}
    {
    }

    auto TOB::available_connectors_bump_to_track(std::usize bump_index) -> std::Vector<TOBConnector> {
        return this->available_connectors(bump_index, TOBSignalDirection::BumpToTrack);
    }

    auto TOB::available_connectors_track_to_bump(std::usize bump_index) -> std::Vector<TOBConnector> {
        return this->available_connectors(bump_index, TOBSignalDirection::TrackToBump);
    }

    auto TOB::available_connectors(std::usize bump_index, TOBSignalDirection signal_dir) -> std::Vector<TOBConnector> {
        assert(bump_index < TOB::INDEX_SIZE);
        assert(signal_dir != TOBSignalDirection::DisConnected);

        auto cs = std::Vector<TOBConnector>{};

        auto bump_to_hori_info = TOB::bump_to_hori_mux_info(bump_index);
        
        for (auto& bump_to_hori_cs : this->_bump_to_hori_muxs.at(std::get<0>(bump_to_hori_info))->available_connectors(std::get<1>(bump_to_hori_info))) {

            auto hori_index = TOB::bump_to_hori_mux_info_and_output_to_index(bump_to_hori_info, bump_to_hori_cs.output_index());
            auto hori_to_vert_info = TOB::hori_to_vert_mux_info(hori_index);

            for (auto& hori_to_vert_cs : this->_hori_to_vert_muxs.at(std::get<0>(hori_to_vert_info))->available_connectors(std::get<1>(hori_to_vert_info))) {
                
                auto vert_index = TOB::hori_to_vert_mux_info_and_output_to_index(hori_to_vert_info, hori_to_vert_cs.output_index());
                auto vert_to_track_info = TOB::vert_to_track_mux_info(vert_index);

                for (auto& vert_to_track_cs : this->_vert_to_track_muxs.at(std::get<0>(vert_to_track_info))->available_connectors(std::get<1>(vert_to_track_info))) {
                        
                    auto track_index = TOB::vert_to_track_mux_info_and_output_to_index(vert_to_track_info, vert_to_track_cs.output_index());
                    assert(track_index < TOB::INDEX_SIZE);

                    auto& bump_dir_reg = this->_bump_dir_registers.at(bump_index);
                    auto& track_dir_reg = this->_track_dir_registers.at(track_index);

                    cs.emplace_back(
                        bump_index, 
                        hori_index,
                        vert_index,
                        track_index,
                        bump_to_hori_cs,
                        hori_to_vert_cs,
                        vert_to_track_cs,
                        bump_dir_reg.get(),
                        track_dir_reg.get(),
                        signal_dir
                    );
                }
            } 
        }
    
        return cs;
    }

    auto TOB::bump_index_map_track_index(std::usize bump_index) const -> std::Option<std::usize> {
        auto bump_to_hori_res = this->bump_index_map_hori_index(bump_index);
        if (!bump_to_hori_res.has_value()) {
            return std::nullopt;
        }

        auto hori_index = *bump_to_hori_res;

        auto hori_to_vert_res = this->hori_index_map_vert_index(hori_index);
        if (!hori_to_vert_res.has_value()) {
            return std::nullopt;
        }

        auto vert_index = *hori_to_vert_res;
        return this->vert_index_map_track_index(vert_index);
    }

    auto TOB::bump_index_map_hori_index(std::usize bump_index) const -> std::Option<std::usize> {
        auto info = TOB::bump_to_hori_mux_info(bump_index);
        auto [mux_index, mux_inner_index] = info;
        auto res = this->_bump_to_hori_muxs.at(mux_index)->index_map(mux_inner_index);
        if (res.has_value()) {
            return { TOB::bump_to_hori_mux_info_and_output_to_index(info, *res) };
        } else {
            return std::nullopt;
        }
    }

    auto TOB::hori_index_map_vert_index(std::usize hori_index) const -> std::Option<std::usize> {
        auto info = TOB::hori_to_vert_mux_info(hori_index);
        auto [mux_index, mux_inner_index] = info;
        auto res = this->_hori_to_vert_muxs.at(mux_index)->index_map(mux_inner_index);
        if (res.has_value()) {
            return { TOB::hori_to_vert_mux_info_and_output_to_index(info, *res) };
        } else {
            return std::nullopt;
        }
    }

    auto TOB::vert_index_map_track_index(std::usize vert_index) const -> std::Option<std::usize> {
        auto info = TOB::vert_to_track_mux_info(vert_index);
        auto [mux_index, mux_inner_index] = info;
        auto res = this->_vert_to_track_muxs.at(mux_index)->index_map(mux_inner_index);
        if (res.has_value()) {
            return { TOB::vert_to_track_mux_info_and_output_to_index(info, *res) };
        } else {
            return std::nullopt;
        }
    }

    auto TOB::bump_to_hori_register_nth(std::usize mux_index, std::usize mux_inner_index) -> TOBMuxRegister* {
        return const_cast<TOBMuxRegister*>(const_cast<TOB*>(this)->bump_to_hori_register_nth(mux_index, mux_inner_index));
    }

    auto TOB::hori_to_vert_register_nth(std::usize mux_index, std::usize mux_inner_index) -> TOBMuxRegister* {
        return const_cast<TOBMuxRegister*>(const_cast<TOB*>(this)->hori_to_vert_register_nth(mux_index, mux_inner_index));
    }

    auto TOB::vert_to_track_register_nth(std::usize mux_index, std::usize mux_inner_index) -> TOBMuxRegister* {
        return const_cast<TOBMuxRegister*>(const_cast<TOB*>(this)->vert_to_track_register_nth(mux_index, mux_inner_index));
    }

    auto TOB::bump_to_hori_register(std::usize bump_index) -> TOBMuxRegister* {
        return const_cast<TOBMuxRegister*>(const_cast<TOB*>(this)->bump_to_hori_register(bump_index));
    }

    auto TOB::hori_to_vert_register(std::usize hori_index) -> TOBMuxRegister* {
        return const_cast<TOBMuxRegister*>(const_cast<TOB*>(this)->hori_to_vert_register(hori_index));
    }

    auto TOB::vert_to_track_register(std::usize vert_index) -> TOBMuxRegister* {
        return const_cast<TOBMuxRegister*>(const_cast<TOB*>(this)->vert_to_track_register(vert_index));
    }

    auto TOB::bump_to_hori_register_nth(std::usize mux_index, std::usize mux_inner_index) const -> const TOBMuxRegister* {
        assert(mux_index < TOB::BUMP_TO_HORI_MUX_COUNT);
        return this->_bump_to_hori_muxs.at(mux_index)->registerr(mux_inner_index);
    }

    auto TOB::hori_to_vert_register_nth(std::usize mux_index, std::usize mux_inner_index) const -> const TOBMuxRegister* {
        assert(mux_index < TOB::HORI_TO_VERI_MUX_COUNT);
        return this->_hori_to_vert_muxs.at(mux_index)->registerr(mux_inner_index);
    }

    auto TOB::vert_to_track_register_nth(std::usize mux_index, std::usize mux_inner_index) const -> const TOBMuxRegister* {
        assert(mux_index < TOB::VERI_TO_TRACK_MUX_COUNT);
        return this->_vert_to_track_muxs.at(mux_index)->registerr(mux_inner_index);
    }

    auto TOB::bump_to_hori_register(std::usize bump_index) const -> const TOBMuxRegister* {
        auto [mux_index, mux_inner_index] = TOB::bump_to_hori_mux_info(bump_index);
        return this->bump_to_hori_register_nth(mux_index, mux_inner_index);
    }

    auto TOB::hori_to_vert_register(std::usize hori_index) const -> const TOBMuxRegister* {
        auto [mux_index, mux_inner_index] = TOB::hori_to_vert_mux_info(hori_index);
        return this->hori_to_vert_register_nth(mux_index, mux_inner_index);
    }

    auto TOB::vert_to_track_register(std::usize vert_index) const -> const TOBMuxRegister* {
        auto [mux_index, mux_inner_index] = TOB::vert_to_track_mux_info(vert_index);
        return this->vert_to_track_register_nth(mux_index, mux_inner_index);
    }

    auto TOB::randomly_map_remain_indexes() -> void {
        for (auto& mux : this->_bump_to_hori_muxs) {
            mux->randomly_map_remain_indexes();
        }

        for (auto& mux : this->_hori_to_vert_muxs) {
            mux->randomly_map_remain_indexes();
        }

        for (auto& mux : this->_vert_to_track_muxs) {
            mux->randomly_map_remain_indexes();
        }
    }

    auto TOB::get_bump(std::usize bump_index) -> std::Option<Bump*> {
        if (bump_index >= TOB::INDEX_SIZE) {
            return std::nullopt;
        }
        return this->_bumps.emplace(
            bump_index, 
            std::make_unique<Bump>(BumpCoord{this->_coord_in_interposer, bump_index}, this)
        ).first->second.get();
    }

    auto TOB::bump_to_hori_mux_info(std::usize bump_index) -> std::Tuple<std::usize, std::usize> {
        return {
            bump_index / TOB::BUMP_TO_HORI_MUX_SIZE,
            bump_index % TOB::BUMP_TO_HORI_MUX_SIZE,  
        };
    }

    auto TOB::bump_to_hori_mux_info_and_output_to_index(std::Tuple<std::usize, std::usize> info, std::usize output) -> std::usize {
        auto [mux_index, _] = info;
        return mux_index * TOB::BUMP_TO_HORI_MUX_SIZE + output;
    }

    auto TOB::hori_to_vert_mux_info(std::usize hori_index) -> std::Tuple<std::usize, std::usize> {
        if (hori_index >= (TOB::INDEX_SIZE / 2)) {
            return {
                hori_index % TOB::HORI_TO_VERI_MUX_SIZE + (TOB::HORI_TO_VERI_MUX_COUNT / 2),
                (hori_index - TOB::INDEX_SIZE / 2) / TOB::HORI_TO_VERI_MUX_SIZE
            };
        } else {
            return {
                hori_index % TOB::HORI_TO_VERI_MUX_SIZE,
                hori_index / TOB::HORI_TO_VERI_MUX_SIZE, 
            };
        }
    }

    auto TOB::hori_to_vert_mux_info_and_output_to_index(std::Tuple<std::usize, std::usize> info, std::usize output) -> std::usize {
        auto [mux_index, _] = info;
        return mux_index * TOB::HORI_TO_VERI_MUX_SIZE + output;
    }

    auto TOB::vert_to_track_mux_info(std::usize vert_index) -> std::Tuple<std::usize, std::usize> {
        if (vert_index >= (TOB::INDEX_SIZE / 2)) {
            return {
                vert_index - (TOB::INDEX_SIZE / 2),
                1
            };
        } else {
            return {
                vert_index,
                0
            };
        }
    }

    auto TOB::vert_to_track_mux_info_and_output_to_index(std::Tuple<std::usize, std::usize> info, std::usize output) -> std::usize {
        auto [mux_index, _] = info;
        return mux_index + (TOB::INDEX_SIZE / 2) * output;
    }

    auto TOB::hori_mux_reg_value(std::usize index) const -> std::Option<std::usize> {
        TOB::check_index(index);

        auto group = index / 8;
        auto gourp_posi = index % 8;

        auto& mux = this->_bump_to_hori_muxs.at(group);
        return mux->index_map(gourp_posi);
    }

    auto TOB::vert_mux_reg_value(std::usize index) const -> std::Option<std::usize> {
        TOB::check_index(index);

        auto group = index / 8;
        auto gourp_posi = index % 8;

        auto& mux = _hori_to_vert_muxs.at(group);
        return mux->index_map(gourp_posi);
    } 

    auto TOB::track_mux_reg_value(std::usize index) const -> std::usize {
        if (index >= 64)
            debug::exception_fmt("track mux index should be in the range of [0, 64]");

        auto& mux = this->_vert_to_track_muxs.at(index);
        auto mux_register0 = mux->registerr(0)->get();
        auto mux_register1 = mux->registerr(1)->get();
        if (mux_register0 == std::nullopt || mux_register1 == std::nullopt)
            throw std::runtime_error(std::format("unmapped track mux at index = {}", index));
        
        if (mux_register0.value() == 0 && mux_register1.value() == 1)
            return 0;
        else if (mux_register0.value() == 1 && mux_register1.value() == 0)
            return 1;
        else{
            throw std::runtime_error(std::format("Unknown config on track mux, with register = [{}, {}]", mux_register0.value(), mux_register1.value()));
        }
    } 
    
    auto TOB::bump_dir_reg_value(std::usize index) const -> TOBBumpDirection {
        TOB::check_index(index);
        return this->_bump_dir_registers.at(index)->get();
    }

    auto TOB::track_dir_reg_value(std::usize index) const -> TOBTrackDirection {
        TOB::check_index(index);
        return this->_track_dir_registers.at(index)->get();
    }

    auto TOB::collect_cobunit_usage() -> void {
        for (auto& [index, bump]: this->_bumps){
            for (auto cobunit: bump->accessable_cobunit()){
                this->_cobunit_resources[cobunit] += 1;
            }
        }
    }

    auto TOB::check_index(std::usize index) -> void {
        if (index >= TOB::INDEX_SIZE)
            debug::exception_fmt("horizontal mux index should be in the range of [0, {})", static_cast<int>(TOB::INDEX_SIZE));
    }
}