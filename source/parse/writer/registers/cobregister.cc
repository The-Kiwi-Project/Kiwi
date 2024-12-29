#include "./cobregister.hh"
#include "./registervalues.hh"

#include <hardware/interposer.hh>



namespace kiwi::parse
{
    CobRegister::~CobRegister() noexcept
    {
        this->_pinterposer = nullptr;
    }

    auto CobRegister::fetch_controlbits(RegisterValue& rv) -> void
    {
        fetch_right_sel(rv);
        fetch_left_sel(rv);
        fetch_up_sel(rv);
        fetch_down_sel(rv);
        fetch_sw_ru(rv);
        fetch_sw_lu(rv);
        fetch_sw_rd(rv);
        fetch_sw_ld(rv);
        fetch_sw_h(rv);
        fetch_sw_v(rv);
    }

    auto CobRegister::fetch_selection(hardware::COB* pcob, std::Bits<128>& sel_register) -> void
    {
        for (std::usize index = 0; index < 128; ++index)
        {
            auto value = pcob->get_sel_resgiter_value(hardware::COBDirection::Right, index);
            if (value == hardware::COBSignalDirection::TrackToCOB)
                sel_register[index] = 0;
            else
                sel_register[index] = 1;
        }
    }

    auto CobRegister::fetch_switch(hardware::COB* pcob, std::Bits<128>& sw_register, \
                                    hardware::COBDirection from_dir, hardware::COBDirection to_dir) -> void
    {
        for (std::usize index = 0; index < 128; ++index)
        {
            auto value = pcob->get_sw_resgiter_value(from_dir, index, to_dir);
            if (value == hardware::COBSwState::Connected)
                sw_register[index] = 1;
            else
                sw_register[index] = 0;
        }
    }

    auto CobRegister::fetch_template(RegisterValue& rv, std::String method) -> void
    {
        for (std::i64 cobrow = 0; cobrow < hardware::Interposer::COB_ARRAY_HEIGHT; ++cobrow)   
        {
            for (std::i64 cobcol = 0; cobcol < hardware::Interposer::COB_ARRAY_WIDTH; ++cobcol)  
            {
                hardware::COBCoord cobcoord {cobrow, cobcol};
                if (!rv.cobs.contains(cobcoord))
                {
                    throw std::runtime_error(std::format("CobRegister::fetch_template(): cannot find cob at ({}, {}) in struct RegisterValue.cobs", cobrow, cobcol));
                }
                else{
                    auto& cob_value {rv.cobs.at(cobcoord)};
                    auto opt_cob = _pinterposer->get_cob(cobcoord);
                    if (opt_cob == std::nullopt)
                        throw std::runtime_error(std::format("CobRegister::fetch_template(): cannot find cob at ({}, {}) in interposer.cobs", cobrow, cobcol));
                    auto cob = opt_cob.value();

                    if (method == "right_sel")
                        fetch_selection(cob, cob_value.right_sel);
                    else if (method == "left_sel")
                        fetch_selection(cob, cob_value.left_sel);
                    else if (method == "up_sel")
                        fetch_selection(cob, cob_value.up_sel);
                    else if (method == "down_sel")
                        fetch_selection(cob, cob_value.down_sel);
                    else if (method == "sw_ru")
                        fetch_switch(cob, cob_value.sw_ru, hardware::COBDirection::Right, hardware::COBDirection::Up);
                    else if (method == "sw_lu")
                        fetch_switch(cob, cob_value.sw_lu, hardware::COBDirection::Left, hardware::COBDirection::Up);
                    else if (method == "sw_rd")
                        fetch_switch(cob, cob_value.sw_rd, hardware::COBDirection::Right, hardware::COBDirection::Down);
                    else if (method == "sw_ld")
                        fetch_switch(cob, cob_value.sw_ld, hardware::COBDirection::Left, hardware::COBDirection::Down);
                    else if (method == "sw_h")
                        fetch_switch(cob, cob_value.sw_h, hardware::COBDirection::Right, hardware::COBDirection::Left);
                    else if (method == "sw_v")
                        fetch_switch(cob, cob_value.sw_v, hardware::COBDirection::Up, hardware::COBDirection::Down);
                    else
                        throw std::runtime_error("illegal cob register type");
                }
            }
        }
    }

    auto CobRegister::fetch_right_sel(RegisterValue& rv) -> void
    {
        fetch_template(rv, "right_sel");
    }

    auto CobRegister::fetch_left_sel(RegisterValue& rv) -> void
    {
        fetch_template(rv, "left_sel");
    }

    auto CobRegister::fetch_up_sel(RegisterValue& rv) -> void
    {
        fetch_template(rv, "up_sel");
    }

    auto CobRegister::fetch_down_sel(RegisterValue& rv) -> void
    {
        fetch_template(rv, "down_sel");
    }

    auto CobRegister::fetch_sw_ru(RegisterValue& rv) -> void
    {
        fetch_template(rv, "sw_ru");
    }

    auto CobRegister::fetch_sw_lu(RegisterValue& rv) -> void
    {
        fetch_template(rv, "sw_lu");
    }

    auto CobRegister::fetch_sw_rd(RegisterValue& rv) -> void
    {
        fetch_template(rv, "sw_rd");
    }

    auto CobRegister::fetch_sw_ld(RegisterValue& rv) -> void
    {
        fetch_template(rv, "sw_ld");
    }

    auto CobRegister::fetch_sw_h(RegisterValue& rv) -> void
    {
        fetch_template(rv, "sw_h");
    }

    auto CobRegister::fetch_sw_v(RegisterValue& rv) -> void
    {
        fetch_template(rv, "sw_v");
    }
}

