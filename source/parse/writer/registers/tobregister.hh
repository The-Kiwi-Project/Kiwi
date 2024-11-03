#pragma once


#include "./baseregister.hh"
#include <std/file.hh>
#include <std/memory.hh>
#include <std/collection.hh>
#include <functional>


namespace kiwi::parse
{
    struct RegisterValue;
    class TobValue;

    class TobRegister : public BaseRegister
    {
    public:
        TobRegister(hardware::Interposer* pinterposer):
            _pinterposer{pinterposer}
        {}

        ~TobRegister() noexcept override;

        using FETCH_FUNCTION_PARAMS = std::Tuple<hardware::TOB*, parse::TobValue&, hardware::TOBCoord&>;

    public:
        auto fetch_controlbits(RegisterValue& rv) -> void override;

    private:
        auto fetch_bump_direction(RegisterValue& rv) -> void;
        auto fetch_track_direction(RegisterValue& rv) -> void;
        auto fetch_dly(RegisterValue& rv) -> void;
        auto fetch_drv(RegisterValue& rv) -> void;
        auto fetch_hctrl(RegisterValue& rv) -> void;
        auto fetch_vctrl(RegisterValue& rv) -> void;
        auto fetch_bank_sel(RegisterValue& rv) -> void;
        
    private:
        auto fetch_template(RegisterValue& rv, std::String method) -> void;
        auto map_empty_mux() -> void;
        static std::HashMap<std::String, std::function<void(FETCH_FUNCTION_PARAMS)>> fetch_methods;
    
    private:
        hardware::Interposer* _pinterposer;
    };
} 


