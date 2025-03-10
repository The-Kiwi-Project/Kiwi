#pragma once 

#include "./tobmux.hh"
#include "./tobconnector.hh"
#include "./tobsigdir.hh"
#include <hardware/coord.hh>
#include <hardware/bump/bump.hh>

#include <std/memory.hh>
#include <std/collection.hh>
#include <std/integer.hh>
#include <std/utility.hh>
#include <hardware/tob/tobcoord.hh>
#include <hardware/tob/tobregister.hh>

namespace kiwi::circuit {
    class TopDieInstance;
}

namespace kiwi::hardware {

    class TOB { 
    public:
        enum {
            INDEX_SIZE = 128,

            BUMP_TO_HORI_MUX_SIZE = 8,
            BUMP_TO_HORI_MUX_COUNT = 16,

            HORI_TO_VERI_MUX_SIZE = 8,
            HORI_TO_VERI_MUX_COUNT = 16,

            VERI_TO_TRACK_MUX_SIZE = 2,
            VERI_TO_TRACK_MUX_COUNT = 64,
        };

        static_assert(BUMP_TO_HORI_MUX_SIZE * BUMP_TO_HORI_MUX_COUNT == INDEX_SIZE);
        static_assert(HORI_TO_VERI_MUX_SIZE * HORI_TO_VERI_MUX_COUNT == INDEX_SIZE);
        static_assert(VERI_TO_TRACK_MUX_SIZE * VERI_TO_TRACK_MUX_COUNT == INDEX_SIZE);

    public:
        TOB(TOBCoord const& coord, Coord const& coord_in_interposer);
        TOB(std::i64 row, std::i64 col);

    public:
        auto available_connectors_bump_to_track(std::usize bump_index) -> std::Vector<TOBConnector>;
        auto available_connectors_track_to_bump(std::usize bump_index) -> std::Vector<TOBConnector>;

        auto available_connectors(std::usize bump_index, TOBSignalDirection dir) -> std::Vector<TOBConnector>;

    public:
        auto bump_index_map_track_index(std::usize bump_index) const -> std::Option<std::usize>;

        auto bump_index_map_hori_index(std::usize bump_index) const -> std::Option<std::usize>;
        auto hori_index_map_vert_index(std::usize hori_index) const -> std::Option<std::usize>;
        auto vert_index_map_track_index(std::usize vert_index) const -> std::Option<std::usize>;

    public:
        auto bump_to_hori_register_nth(std::usize mux_index, std::usize mux_inner_index) -> TOBMuxRegister*;
        auto hori_to_vert_register_nth(std::usize mux_index, std::usize mux_inner_index) -> TOBMuxRegister*;
        auto vert_to_track_register_nth(std::usize mux_index, std::usize mux_inner_index) -> TOBMuxRegister*;
        auto bump_to_hori_register(std::usize bump_index) -> TOBMuxRegister*;
        auto hori_to_vert_register(std::usize hori_index) -> TOBMuxRegister*;
        auto vert_to_track_register(std::usize vert_index) -> TOBMuxRegister*;

        auto bump_to_hori_register_nth(std::usize mux_index, std::usize mux_inner_index) const -> const TOBMuxRegister*;
        auto hori_to_vert_register_nth(std::usize mux_index, std::usize mux_inner_index) const -> const TOBMuxRegister*;
        auto vert_to_track_register_nth(std::usize mux_index, std::usize mux_inner_index) const -> const TOBMuxRegister*;
        auto bump_to_hori_register(std::usize bump_index) const -> const TOBMuxRegister*;
        auto hori_to_vert_register(std::usize hori_index) const -> const TOBMuxRegister*;
        auto vert_to_track_register(std::usize vert_index) const -> const TOBMuxRegister*;

    public:
        auto randomly_map_remain_indexes() -> void;

    public:
        static auto bump_to_hori_mux_info(std::usize bump_index) -> std::Tuple<std::usize, std::usize>;
        static auto bump_to_hori_mux_info_and_output_to_index(std::Tuple<std::usize, std::usize> info, std::usize output) -> std::usize;
        static auto hori_to_vert_mux_info(std::usize hori_index) -> std::Tuple<std::usize, std::usize>;
        static auto hori_to_vert_mux_info_and_output_to_index(std::Tuple<std::usize, std::usize> info, std::usize output) -> std::usize;
        static auto vert_to_track_mux_info(std::usize vert_index) -> std::Tuple<std::usize, std::usize>;
        static auto vert_to_track_mux_info_and_output_to_index(std::Tuple<std::usize, std::usize> info, std::usize output) -> std::usize;

    public:
        auto hori_mux_reg_value(std::usize index) const -> std::Option<std::usize>;
        auto vert_mux_reg_value(std::usize index) const -> std::Option<std::usize>;
        auto track_mux_reg_value(std::usize index) const -> std::usize;
        auto bump_dir_reg_value(std::usize index) const -> TOBBumpDirection;
        auto track_dir_reg_value(std::usize index) const -> TOBTrackDirection;

        auto get_bump(std::usize bump_index) -> std::Option<Bump*>;
        auto cobunit_resources() -> std::Array<std::usize, 16>& {return this->_cobunit_resources;}
    
    public:
        auto collect_cobunit_usage() -> void;

    private:
        static auto check_index(std::usize index) -> void;

    public:
        auto coord() const -> TOBCoord const& 
        { return this->_coord; }
        
        auto coord_in_interposer() const -> Coord const& 
        { return this->_coord_in_interposer; }
        
        auto placed_instance() const -> circuit::TopDieInstance*
        { return this->_placed_instance.value(); }

        void set_placed_instance(circuit::TopDieInstance* instance) 
        { this->_placed_instance.emplace(instance); } 

        void remove_placed_instance() 
        { this->_placed_instance.reset(); }

        auto is_idle() const -> bool 
        { return !this->_placed_instance.has_value(); }

    private:
        TOBCoord const _coord;
        Coord const _coord_in_interposer;

        std::Option<circuit::TopDieInstance*> _placed_instance {};

        std::HashMap<std::usize, std::Box<Bump>> _bumps {};
        std::Array<std::usize, 16> _cobunit_resources;

        std::Vector<std::Box<TOBMux>> _bump_to_hori_muxs {};
        std::Vector<std::Box<TOBMux>> _hori_to_vert_muxs {};
        std::Vector<std::Box<TOBMux>> _vert_to_track_muxs {};

        std::Vector<std::Box<TOBBumpDirRegister>>  _bump_dir_registers {};
        std::Vector<std::Box<TOBTrackDirRegister>> _track_dir_registers {}; 
    };

}