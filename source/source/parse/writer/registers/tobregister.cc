#include "./tobregister.hh"
#include "./registervalues.hh"

#include <hardware/interposer.hh>
#include <hardware/tob/tobcoord.hh>
#include <format>


namespace kiwi::parse
{
    TobRegister::~TobRegister() noexcept
    { 
        _pinterposer = nullptr; // unique pointer to interposer is in Builder
    }

    std::HashMap<std::String, std::function<void(TobRegister::FETCH_FUNCTION_PARAMS)>> TobRegister::fetch_methods {
        {
            "bump_direction", [](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 128; ++index)
                {
                    auto value = ptob->bump_dir_reg_value(index);
                    if (value == hardware::TOBBumpDirection::DisConnected)
                    {
                        tob_value.tob2bump[index] = 0;
                        tob_value.bump2tob[index] = 0;
                    }
                    else
                    {
                        if (value == hardware::TOBBumpDirection::BumpToTOB)
                        {
                            tob_value.tob2bump[index] = 0;
                            tob_value.bump2tob[index] = 1;
                        }
                        else
                        {
                            tob_value.tob2bump[index] = 1;
                            tob_value.bump2tob[index] = 0;
                        }
                    }
                }
            }
        },
        {
            "track_direction" ,[](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 128; ++index)
                {
                    auto value = ptob->track_dir_reg_value(index);
                    if (value == hardware::TOBTrackDirection::DisConnected)
                    {
                        tob_value.track2tob[index] = 0;
                        tob_value.tob2track[index] = 0;
                    }
                    else
                    {
                        if (value == hardware::TOBTrackDirection::TrackToTOB)
                        {
                            tob_value.track2tob[index] = 1;
                            tob_value.tob2track[index] = 0;
                        }
                        else
                        {
                            tob_value.tob2track[index] = 1;
                            tob_value.track2tob[index] = 0;
                        }
                    }
                }
            }
        },
        {
            "dly", [](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 64; ++index)
                {
                    tob_value.dly[index] = 0;
                }
            }
        },
        {
            "drv", [](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 64; ++index)
                {
                    tob_value.drv[index] = 0;
                }
            }
        },
        {
            "horizontal", [](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 128; ++index) {
                    auto value = ptob->hori_mux_reg_value(index);
                    if (!value.has_value())
                        throw std::runtime_error(std::format("horizontal register value is null optional at index = {}", index));
                    tob_value.hctrl[index] = value.value();
                }
            }
        },
        {
            "vertical", [](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 128; ++index) {
                    auto value = ptob->vert_mux_reg_value(index);
                    if (!value.has_value())
                        throw std::runtime_error(std::format("horizontal register value is null optional at index = {}", index));
                    tob_value.vctrl[index] = value.value();
                }
            }
        },
        {
            "bank_sel", [](FETCH_FUNCTION_PARAMS params) -> void
            {
                auto [ptob, tob_value, tobcoord] = params;
                for (std::usize index = 0; index < 64; ++index) {
                    auto value = ptob->track_mux_reg_value(index);
                    tob_value.bank_mux[index] = value;
                }

            }
        }
    };

    auto TobRegister::fetch_template(RegisterValue& rv, std::String method) -> void
    {
        for (std::i64 tobrow = 0; tobrow < hardware::Interposer::TOB_ARRAY_HEIGHT; ++tobrow)   
        {
            for (std::i64 tobcol = 0; tobcol < hardware::Interposer::TOB_ARRAY_WIDTH; ++tobcol)  
            {
                hardware::TOBCoord tobcoord {tobrow, tobcol};
                if (!rv.tobs.contains(tobcoord))
                {
                    throw std::runtime_error(std::format("cannot find tob at ({}, {}) in struct RegisterValue.tobs", tobrow, tobcol));
                }
                else{
                    auto& tob_value {rv.tobs.at(tobcoord)};
                    auto opt_tob = _pinterposer->get_tob(tobcoord);
                    if (opt_tob == std::nullopt)
                        throw std::runtime_error(std::format("cannot find tob at ({}, {}) in interposer.tobs", tobrow, tobcol));
                    auto tob = opt_tob.value();
                    
                    if (!fetch_methods.contains(method))
                    {
                        throw std::runtime_error(std::format("using illegal fetch methods \"{}\" when fetching register values", method));
                    }
                    else{
                        try{
                        fetch_methods.at(method)(FETCH_FUNCTION_PARAMS{tob, tob_value, tobcoord});
                        }
                        catch(std::exception& e)
                        {
                            throw std::runtime_error(std::format("TobRegister::fetch_template(), method={} >> ", method) + std::String(e.what()));
                        }
                    }
                    
                }
            }
        }
    }

    auto TobRegister::fetch_controlbits(RegisterValue& rv) -> void
    {
        map_empty_mux();

        fetch_bump_direction(rv);
        fetch_track_direction(rv);
        fetch_dly(rv);
        fetch_drv(rv);
        fetch_hctrl(rv);
        fetch_vctrl(rv);
        fetch_bank_sel(rv);
    }

    auto TobRegister::fetch_bump_direction(RegisterValue& rv) -> void
    {
        fetch_template(rv, "bump_direction");
    }

    auto TobRegister::fetch_track_direction(RegisterValue& rv) -> void
    {
        fetch_template(rv, "track_direction");
    }

    auto TobRegister::fetch_dly(RegisterValue& rv) -> void
    {
        fetch_template(rv, "dly");
    }

    auto TobRegister::fetch_drv(RegisterValue& rv) -> void
    {
        fetch_template(rv, "drv");
    }

    auto TobRegister::fetch_hctrl(RegisterValue& rv) -> void
    {
        fetch_template(rv, "horizontal");
    }

    auto TobRegister::fetch_vctrl(RegisterValue& rv) -> void
    {
        fetch_template(rv, "vertical");
    }

    auto TobRegister::fetch_bank_sel(RegisterValue& rv) -> void
    {
        fetch_template(rv, "bank_sel");
    }

    auto TobRegister::map_empty_mux() -> void 
    {
        for (std::i64 row = 0; row < hardware::Interposer::TOB_ARRAY_WIDTH; ++row)
        {
            for (std::i64 col = 0; col < hardware::Interposer::TOB_ARRAY_HEIGHT; ++col)
            {
                hardware::TOBCoord tobcoord {row, col};
                const auto& tob = _pinterposer->get_tob(tobcoord);
                if (tob == std::nullopt)
                    throw std::runtime_error(std::format("get empty pointer to tob at ({}, {})", row, col));
                
                tob.value()->randomly_map_remain_indexes();
            }
        }
    }

}


