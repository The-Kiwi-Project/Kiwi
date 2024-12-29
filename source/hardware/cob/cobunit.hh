#pragma once 

#include "./cobregister.hh"
#include "./cobdirection.hh"

#include <std/integer.hh>
#include <std/memory.hh>
#include <std/collection.hh>

namespace kiwi::hardware {

    struct COBUnitConnector {
        COBDirection    from_dir;
        std::usize      from_index;

        COBDirection    to_dir;
        std::usize      to_index;

        COBSwRegister*  sw_reg;

        COBSelRegister* t_to_c_sel_reg;
        COBSelRegister* c_to_t_sel_reg;

        auto is_connected() -> bool {
            return this->sw_reg;
        }
    };

    class COBUnit {
    public:
        enum { WILTON_SIZE = 8 };

    public:
        COBUnit() = default;

    public:
        auto adjacent_connectors(COBDirection from_dir, std::usize from_index) -> std::Vector<COBUnitConnector>;
        
        auto adjacent_connectors_from_left(std::usize index) -> std::Vector<COBUnitConnector>;
        auto adjacent_connectors_from_right(std::usize index) -> std::Vector<COBUnitConnector>;
        auto adjacent_connectors_from_up(std::usize index) -> std::Vector<COBUnitConnector>;
        auto adjacent_connectors_from_down(std::usize index) -> std::Vector<COBUnitConnector>;

        auto get_connector(COBDirection from_dir, std::usize from_index, COBDirection to_dir) -> COBUnitConnector;

    public:
        auto sw_register(COBDirection from_dir, std::usize from_index, COBDirection to_dir) -> COBSwRegister*;
        auto sel_register(COBDirection dir, std::usize index) -> COBSelRegister*;

    public:
        static auto index_map(COBDirection from_dir, std::usize from_index, COBDirection to_dir) -> std::usize;
        
        static auto left_index_map_up_index(std::usize index) -> std::usize;
        static auto up_index_map_left_index(std::usize index) -> std::usize;
        static auto left_index_map_down_index(std::usize index) -> std::usize;
        static auto down_index_map_left_index(std::usize index) -> std::usize;
        static auto right_index_map_up_index(std::usize index) -> std::usize;
        static auto up_index_right_up_index(std::usize index) -> std::usize;
        static auto right_index_map_down_index(std::usize index) -> std::usize;
        static auto down_index_map_right_index(std::usize index) -> std::usize;
        static auto left_index_map_right_index(std::usize index) -> std::usize;
        static auto right_index_map_left_index(std::usize index) -> std::usize;
        static auto up_index_map_down_index(std::usize index) -> std::usize;
        static auto down_index_map_up_index(std::usize index) -> std::usize;
        
        static auto assert_index(std::usize index) -> void;

    private:
        std::Vector<COBSwRegister> _switch_ru {WILTON_SIZE};
        std::Vector<COBSwRegister> _switch_lu {WILTON_SIZE};
        std::Vector<COBSwRegister> _switch_rd {WILTON_SIZE};
        std::Vector<COBSwRegister> _switch_ld {WILTON_SIZE};
        std::Vector<COBSwRegister> _switch_lr {WILTON_SIZE};
        std::Vector<COBSwRegister> _switch_ud {WILTON_SIZE};

        std::Vector<COBSelRegister> _left_sel {WILTON_SIZE};
        std::Vector<COBSelRegister> _right_sel {WILTON_SIZE};
        std::Vector<COBSelRegister> _up_sel {WILTON_SIZE};
        std::Vector<COBSelRegister> _down_sel {WILTON_SIZE};
    };

}