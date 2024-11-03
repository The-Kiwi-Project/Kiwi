#include "./writer.hh"
#include "./registers/tobregister.hh"
#include "./registers/cobregister.hh"
#include <hardware/interposer.hh>

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <iostream>


namespace kiwi::parse
{

    Writer::Writer(hardware::Interposer* pinterposer):
            _rv{},
            _regs{},
            _pinterposer{pinterposer}
        {}

    auto Writer::fetch_and_write(const std::FilePath& file) -> void
    {
        build_regs();
        fetch();
        write(file);
    }

    auto Writer::build_regs() -> void
    {
        TobRegister* ptr_tr = new TobRegister(_pinterposer);
        CobRegister* ptr_cr = new CobRegister(_pinterposer);
        _regs.emplace_back(ptr_tr);
        _regs.emplace_back(ptr_cr);
    }

    auto Writer::fetch() -> void
    {
        for(auto& p_r: _regs)
        {
            p_r->fetch_controlbits(_rv);
        }
    }

    auto Writer::write(const std::FilePath& filepath) -> void
    {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error(std::format("cannon open file {}", filepath.string()));
        }

        write_cob(file);
        write_tob(file);
    }

    auto Writer::write_cob(std::ofstream& file) -> void
    {
        for(std::i64 row = 0; row < hardware::Interposer::COB_ARRAY_HEIGHT; ++row)
        {
            for (std::i64 col = 0; col < hardware::Interposer::COB_ARRAY_WIDTH; ++col)
            {
                hardware::COBCoord cobcoord {row, col};
                if (_rv.cobs.contains(cobcoord))
                {
                    auto& cob_value = _rv.cobs.at(cobcoord);

                    write_cob_template(file, cob_value.right_sel, "right_sel", row, col);
                    write_cob_template(file, cob_value.left_sel, "left_sel", row, col);
                    write_cob_template(file, cob_value.up_sel, "up_sel", row, col);
                    write_cob_template(file, cob_value.down_sel, "down_sel", row, col);
                    write_cob_template(file, cob_value.sw_ru, "sw_ru", row, col);
                    write_cob_template(file, cob_value.sw_lu, "sw_lu", row, col);
                    write_cob_template(file, cob_value.sw_rd, "sw_rd", row, col);
                    write_cob_template(file, cob_value.sw_ld, "sw_ld", row, col);
                    write_cob_template(file, cob_value.sw_v, "sw_v", row, col);
                    write_cob_template(file, cob_value.sw_h, "sw_h", row, col);
                }
                else
                {
                    throw std::runtime_error(std::format("Writer::write_cob(): cannot find cob at ({}, {})", row, col));
                }
            }
        }
    }

    auto Writer::write_tob(std::ofstream& file) -> void
    {
        for(std::i64 row = 0; row < hardware::Interposer::TOB_ARRAY_HEIGHT; ++row)
        {
            for (std::i64 col = 0; col < hardware::Interposer::TOB_ARRAY_WIDTH; ++col)
            {
                hardware::TOBCoord tobcoord {row, col};
                if (_rv.tobs.contains(tobcoord))
                {
                    auto& tob_value {_rv.tobs.at(tobcoord)};

                    only_this_one_looks_f__king_different_from_others(file, tob_value.tob2bump, "tob2bump", row, col);
                    write_tob_template64(file, tob_value.dly, "dly", row, col);
                    write_tob_template64(file, tob_value.dly, "drv", row, col);
                    write_tob_template_mux(file, tob_value.hctrl, "hctrl", row, col);
                    write_tob_template_mux(file, tob_value.vctrl, "vctrl", row, col);
                    write_tob_template64(file, tob_value.bank_mux, "bank_sel", row, col);
                    write_tob_template128(file, tob_value.tob2track, "tob2track", row, col);
                    only_this_one_looks_f__king_different_from_others(file, tob_value.bump2tob, "bump2tob", row, col);
                    write_tob_template128(file, tob_value.track2tob, "track2tob", row, col);
                }
                else
                {
                    throw std::runtime_error(std::format("cannot find cob at ({}, {})", row, col));
                }
            }
        }
    }

    template<std::usize N_bits, std::usize N_parts>
    auto split_bits(const std::Bits<N_bits>& controlbits) -> std::array<std::Bits<N_bits / N_parts>, N_parts>
    {
        static_assert(N_bits % N_parts == 0, "N_bits must be divisible by N_parts");

        std::array<std::Bits<N_bits / N_parts>, N_parts> result;

        for (std::size_t i = 0; i < N_parts; ++i) {
            for (std::size_t j = 0; j < N_bits / N_parts; ++j) {
                result[i][j] = controlbits[i * (N_bits / N_parts) + j];
            }

            std::Bits<N_bits / N_parts> temp {result[i]};
            for (std::usize k = 0; k < temp.size(); ++k)
            {
                result[i][k] = temp[temp.size() - 1 - k];
            }
        }

        // already been reversed, with MSB on the left and LSB on the right
        return result;
    }

    template<std::usize N_bits, std::usize N_parts>
    auto split_array(const std::array<std::size_t, N_bits>& controlbits) -> std::array<std::array<std::size_t, N_bits / N_parts>, N_parts>
    {
        static_assert(N_bits % N_parts == 0, "N must be divisible by N_parts");

        std::array<std::array<std::size_t, N_bits / N_parts>, N_parts> result;

        for (std::size_t i = 0; i < N_parts; ++i) {
            for (std::size_t j = 0; j < N_bits / N_parts; ++j) {
                result[i][j] = controlbits[i * (N_bits / N_parts) + j];
            }

            std::reverse(result[i].begin(), result[i].end());
        }

        // already been reversed, with MSB on the left and LSB on the right
        return result;
    }

    auto Writer::write_cob_template(std::ofstream& file, const std::Bits<128>& bits, \
                                    std::String reg_name, std::usize row, std::usize col) -> void
    {
        auto splitted_bits = split_bits<128, 4>(bits);
        for (std::usize i = 0; i < 4; i++)
        {
            std::String name = std::format("cob_{}_{}_{}_{}", row, col, reg_name, i);
            file << to_hex(splitted_bits[i]) << " " << name << std::endl;
        }
    }

    auto Writer::write_tob_template64(std::ofstream& file, const std::Bits<64>& bits, \
                                    std::String reg_name, std::usize row, std::usize col) -> void
    {
        auto splitted_bits = split_bits<64, 2>(bits);
        for (std::usize i = 0; i < 2; i++)
        {
            std::String name = std::format("tob_{}_{}_{}_{}", row, col, reg_name, i);
            file << to_hex(splitted_bits[i]) << " " << name << std::endl;
        }
    }

    auto Writer::write_tob_template128(std::ofstream& file, const std::Bits<128>& bits, \
                                    std::String reg_name, std::usize row, std::usize col) -> void
    {
        auto splitted_bits = split_bits<128, 4>(bits);
        for (std::usize i = 0; i < 4; i++)
        {
            std::String name = std::format("tob_{}_{}_{}_{}", row, col, reg_name, i);
            file << to_hex(splitted_bits[i]) << " " << name << std::endl;
        }
    }

    auto Writer::write_tob_template_mux(std::ofstream& file, const std::Array<std::usize, 128>& bits, \
                                        std::String reg_name, std::usize row, std::usize col) -> void
    {   
        auto splitted_bits = split_array<128, 16>(bits);
        std::Array<std::Bits<32>, 16> result {};
        for (std::usize outer_i = 0; outer_i < 16; ++outer_i)
        {
            auto& data = splitted_bits[outer_i];
            for (std::size_t i = 0; i < data.size(); ++i) {
                std::bitset<3> bits{data[i]}; 
                for (std::size_t j = 0; j < 3; ++j) {
                    result[outer_i][8 + i * 3 + (2 - j)] = bits[j]; 
                }
            }
        }

        for (std::usize i = 0; i < 16; ++i)
        {
            std::usize bank{i/8}, bank_index{i%8};
            std::String name = std::format("tob_{}_{}_{}_bank{}_{}", row, col, reg_name, bank, bank_index);
            file << to_hex(result[i]) << " " << name << std::endl;
        }
    }

    auto Writer::only_this_one_looks_f__king_different_from_others(std::ofstream& file, const std::Bits<128>& bits,\
                                        std::String reg_name, std::usize row, std::usize col) -> void
    {
        auto splitted_bits = split_bits<128, 4>(bits);
        for (std::usize i = 0; i < 4; i++)
        {
            std::usize bank{i/2}, bank_index{i%2};
            std::String name = std::format("tob_{}_{}_{}_bank{}_en{}", row, col, reg_name, bank, bank_index);
            file << to_hex(splitted_bits[i]) << " " << name << std::endl;
        }
    }

    auto Writer::to_hex(const std::Bits<32>& bits) -> std::String
    {
        try{
            std::String binary {bits.to_string()};
            std::reverse(binary.begin(), binary.end());
            std::stringstream hexStream;
            for (size_t i = 0; i < binary.size(); i += 4) {
                std::string byte = binary.substr(i, 4);
                int value = std::stoi(byte, nullptr, 2); // Convert binary to decimal
                hexStream << std::hex << std::setw(1) << std::setfill('0') << value; // Convert decimal to hex
            }
            return hexStream.str();
        }
        catch(std::exception& e)
        {
            throw std::runtime_error("Writer::to_hex(): " + std::String(e.what()));
        }
    }
} 



