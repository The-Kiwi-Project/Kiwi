#pragma once


#include "./baseregister.hh"

#include <functional>


namespace kiwi::hardware
{
    class Interposer;
}


namespace kiwi::parse
{

    class RegisterValuse;

    class CobRegister : public BaseRegister
    {
    public:
        CobRegister(hardware::Interposer* pinterpsoer):
            _pinterposer{pinterpsoer}
        {}

        ~CobRegister() noexcept;

    public:
        auto fetch_controlbits(RegisterValue& rv) -> void override;

    private:
        auto fetch_right_sel(RegisterValue& rv) -> void;
        auto fetch_left_sel(RegisterValue& rv) -> void;
        auto fetch_up_sel(RegisterValue& rv) -> void;
        auto fetch_down_sel(RegisterValue& rv) -> void;
        auto fetch_sw_ru(RegisterValue& rv) -> void;
        auto fetch_sw_lu(RegisterValue& rv) -> void;
        auto fetch_sw_rd(RegisterValue& rv) -> void;
        auto fetch_sw_ld(RegisterValue& rv) -> void;
        auto fetch_sw_h(RegisterValue& rv) -> void;
        auto fetch_sw_v(RegisterValue& rv) -> void;
    
    private:
        auto fetch_template(RegisterValue& rv, std::String method) -> void;
        auto fetch_selection(hardware::COB*, std::Bits<128>&) -> void;
        auto fetch_switch(hardware::COB*, std::Bits<128>&, hardware::COBDirection from_dir, hardware::COBDirection to_dir) -> void;
    
    private:
        hardware::Interposer* _pinterposer;
    };

}
