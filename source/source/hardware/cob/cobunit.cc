#include "./cobunit.hh"
#include "./cobdirection.hh"

#include <std/collection.hh>
#include <std/integer.hh>
#include <cassert>

namespace kiwi::hardware {

    auto COBUnit::adjacent_connectors(COBDirection from_dir, std::usize from_index) -> std::Vector<COBUnitConnector> {
        COBUnit::assert_index(from_index);
        
        auto connectors = std::Vector<COBUnitConnector>{};
        for (const auto dir : {COBDirection::Left, COBDirection::Right, COBDirection::Up, COBDirection::Down}) {
            if (dir == from_dir) {
                continue;
            }

            connectors.push_back(this->get_connector(from_dir, from_index, dir));
        }

        return connectors;
    }

    auto COBUnit::adjacent_connectors_from_left(std::usize index) -> std::Vector<COBUnitConnector> {
        return this->adjacent_connectors(COBDirection::Left, index);
    }

    auto COBUnit::adjacent_connectors_from_right(std::usize index) -> std::Vector<COBUnitConnector> {
        return this->adjacent_connectors(COBDirection::Right, index);
    }

    auto COBUnit::adjacent_connectors_from_up(std::usize index) -> std::Vector<COBUnitConnector> {
        return this->adjacent_connectors(COBDirection::Up, index);
    }

    auto COBUnit::adjacent_connectors_from_down(std::usize index) -> std::Vector<COBUnitConnector> {
        return this->adjacent_connectors(COBDirection::Down, index);
    }

    auto COBUnit::get_connector(COBDirection from_dir, std::usize from_index, COBDirection to_dir) -> COBUnitConnector {
        COBUnit::assert_index(from_index);
        auto to_index = COBUnit::index_map(from_dir, from_index, to_dir);
        return COBUnitConnector {
            .from_dir = from_dir,
            .from_index = from_index,
            .to_dir = to_dir,
            .to_index = to_index,
            .sw_reg = this->sw_register(from_dir, from_index, to_dir),
            .t_to_c_sel_reg = this->sel_register(from_dir, from_index),
            .c_to_t_sel_reg = this->sel_register(to_dir, to_index)
        };
    }

    auto COBUnit::sw_register(COBDirection from_dir, std::usize from_index, COBDirection to_dir) -> COBSwRegister& {
        COBUnit::assert_index(from_index);

        auto to_index = COBUnit::index_map(from_dir, from_index, to_dir);

        switch (from_dir) {
            case COBDirection::Left: switch (to_dir) {
                case COBDirection::Left:  assert(false);
                case COBDirection::Right: return this->_switch_lr[from_index];
                case COBDirection::Up:    return this->_switch_lu[from_index];
                case COBDirection::Down:  return this->_switch_ld[from_index];
            }
            case COBDirection::Right: switch (to_dir) {
                case COBDirection::Left:  return this->_switch_lr[from_index];
                case COBDirection::Right: assert(false);
                case COBDirection::Up:    return this->_switch_ru[from_index];
                case COBDirection::Down:  return this->_switch_rd[from_index];
            }
            case COBDirection::Up: switch (to_dir) {
                case COBDirection::Left:  return this->_switch_lu[to_index];
                case COBDirection::Right: return this->_switch_ru[to_index];
                case COBDirection::Up:    assert(false);
                case COBDirection::Down:  return this->_switch_ud[to_index];
            }
            case COBDirection::Down: switch (to_dir) {
                case COBDirection::Left:  return this->_switch_ld[to_index];
                case COBDirection::Right: return this->_switch_rd[to_index];
                case COBDirection::Up:    return this->_switch_ud[to_index];
                case COBDirection::Down:  assert(false);
            }
        }

        // Just for clear compiler warning!
        return this->_switch_lu[0];
    }

    auto COBUnit::sel_register(COBDirection dir, std::usize index) -> COBSelRegister& {
        COBUnit::assert_index(index);
        switch (dir) {
            case COBDirection::Left:  return this->_left_sel[index];
            case COBDirection::Right: return this->_right_sel[index];
            case COBDirection::Up:    return this->_up_sel[index];
            case COBDirection::Down:  return this->_down_sel[index];
        }
        // Just for clear compiler warning!
        return this->_left_sel[0];
    }

    auto COBUnit::index_map(COBDirection from_dir, std::usize from_index, COBDirection to_dir) -> std::usize {
        COBUnit::assert_index(from_index);

        switch (from_dir) {
            case COBDirection::Left: switch (to_dir) {
                case COBDirection::Left:  assert(false);
                case COBDirection::Right: return COBUnit::left_index_map_right_index(from_index);
                case COBDirection::Up:    return COBUnit::left_index_map_up_index(from_index);
                case COBDirection::Down:  return COBUnit::left_index_map_down_index(from_index);
            }
            case COBDirection::Right: switch (to_dir) {
                case COBDirection::Left:  return COBUnit::right_index_map_left_index(from_index);
                case COBDirection::Right: assert(false);
                case COBDirection::Up:    return COBUnit::right_index_map_up_index(from_index);
                case COBDirection::Down:  return COBUnit::right_index_map_down_index(from_index);
            }
            case COBDirection::Up: switch (to_dir) {
                case COBDirection::Left:  return COBUnit::up_index_map_left_index(from_index);
                case COBDirection::Right: return COBUnit::up_index_right_up_index(from_index);
                case COBDirection::Up:    assert(false);
                case COBDirection::Down:  return COBUnit::up_index_map_down_index(from_index);
            }
            case COBDirection::Down: switch (to_dir) {
                case COBDirection::Left:  return COBUnit::down_index_map_left_index(from_index);
                case COBDirection::Right: return COBUnit::down_index_map_right_index(from_index);
                case COBDirection::Up:    return COBUnit::down_index_map_up_index(from_index);
                case COBDirection::Down:  assert(false);
            }
        }

        // Unreachable!!
        return -1;
    }

    auto COBUnit::left_index_map_up_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == COBUnit::WILTON_SIZE - 1) {
            return COBUnit::WILTON_SIZE - 1;
        } else {
            return COBUnit::WILTON_SIZE - 2 - index;
        }
    }

    auto COBUnit::up_index_map_left_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == COBUnit::WILTON_SIZE - 1) {
            return COBUnit::WILTON_SIZE - 1;
        } else {
            return COBUnit::WILTON_SIZE - 2 - index;
        }
    }

    auto COBUnit::left_index_map_down_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == COBUnit::WILTON_SIZE - 1) {
            return 0;
        } else {
            return index + 1;
        }
    }

    auto COBUnit::down_index_map_left_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == 0) {
            return COBUnit::WILTON_SIZE - 1;
        } else {
            return index - 1;
        }
    }

    auto COBUnit::right_index_map_up_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == 0) {
            return COBUnit::WILTON_SIZE - 1;
        } else {
            return index - 1;
        }
    }

    auto COBUnit::up_index_right_up_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == COBUnit::WILTON_SIZE - 1) {
            return 0;
        } else {
            return index + 1;
        }
    }

    auto COBUnit::right_index_map_down_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == COBUnit::WILTON_SIZE - 1) {
            return COBUnit::WILTON_SIZE - 1;
        } else {
            return COBUnit::WILTON_SIZE - 2 - index;
        }
    }

    auto COBUnit::down_index_map_right_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        if (index == COBUnit::WILTON_SIZE - 1) {
            return COBUnit::WILTON_SIZE - 1;
        } else {
            return COBUnit::WILTON_SIZE - 2 - index;
        }
    }

    auto COBUnit::left_index_map_right_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        return index;
    }

    auto COBUnit::right_index_map_left_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        return index;
    }  

    auto COBUnit::up_index_map_down_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        return index;
    }

    auto COBUnit::down_index_map_up_index(std::usize index) -> std::usize {
        COBUnit::assert_index(index);
        return index;
    }

    auto COBUnit::assert_index(std::usize index) -> void {
        assert(index < COBUnit::WILTON_SIZE);
    }

}
