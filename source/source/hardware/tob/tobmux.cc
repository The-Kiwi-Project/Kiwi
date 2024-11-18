#include "./tobmux.hh"
#include <std/collection.hh>
#include <std/integer.hh>
#include <cassert>

namespace kiwi::hardware {


    TOBMuxConnector::TOBMuxConnector(
        std::usize input_index, 
        std::usize output_index, 
        TOBMuxRegister& reg
    ) :
        _input_index{input_index},
        _output_index{output_index},
        _register{reg}
    {}


    auto TOBMuxConnector::connect() -> void {
        this->_register.set(this->_output_index);
    }

    auto TOBMuxConnector::disconnect() -> void {
        this->_register.reset();
    }

    ////////////////////////////////////////////////////////////////

    TOBMux::TOBMux(std::usize mux_size) :
        _mux_size{mux_size},
        _registers(mux_size)
    {

    }


    auto TOBMux::available_connectors(std::usize input_index) -> std::Vector<TOBMuxConnector> {
        if (this->_registers.at(input_index).get().has_value()) {
            return {};
        }
    
        auto connectors = std::Vector<TOBMuxConnector>{};
        for (const auto output_index : this->available_output_indexes()) {
            connectors.emplace_back(
                input_index,
                output_index,
                this->_registers[input_index]
            );
        }

        return connectors;
    }

    auto TOBMux::available_output_indexes() -> std::Vector<std::usize> {
        auto useds = std::Vector<bool>(this->_mux_size, false);
        for (const auto& reg : this->_registers) {
            auto res = reg.get();
            if (res.has_value()) {
                useds[*res] = true;
            }
        }

        auto indexes = std::Vector<std::usize>{};
        for (auto i = 0; i < this->_mux_size; ++i) {
            if (useds[i]) {
                continue;
            }
            indexes.emplace_back(i);
        }

        return indexes;
    }

    auto TOBMux::randomly_map_remain_indexes() -> void {
        auto unused_indexes = this->available_output_indexes();
        std::usize index = 0;
        for (auto& reg : this->_registers) {
            if (!reg.get().has_value()) {
                reg.set(unused_indexes[index]);
                index += 1;
            }
        }
        assert(index == unused_indexes.size());
    }

    auto TOBMux::index_map(std::usize input_index) const -> std::Option<std::usize> {
        return this->_registers.at(input_index).get();
    }

    auto TOBMux::registerr(std::usize input_index) -> TOBMuxRegister& {
        return this->_registers.at(input_index);
    }
    
}