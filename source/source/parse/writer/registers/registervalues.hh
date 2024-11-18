#pragma once


#include <std/collection.hh>
#include <std/integer.hh>
#include <hardware/interposer.hh>
#include <hardware/cob/cobcoord.hh>


namespace kiwi::parse
{
    struct TobValue
    {
        std::Bits<128> tob2bump;                
        std::Bits<128> bump2tob;                
        std::Bits<64> drv;                      
        std::Bits<64> dly;
        std::Array<std::usize, 128> hctrl;      
        std::Array<std::usize, 128> vctrl;      
        std::Bits<64> bank_mux;                 
        std::Bits<128> tob2track;               
        std::Bits<128> track2tob;
    };

    struct CobValue
    {
        std::Bits<128> right_sel;
        std::Bits<128> left_sel;
        std::Bits<128> up_sel;
        std::Bits<128> down_sel;
        std::Bits<128> sw_ru;
        std::Bits<128> sw_lu;
        std::Bits<128> sw_rd;
        std::Bits<128> sw_ld;
        std::Bits<128> sw_v;
        std::Bits<128> sw_h;
    };


    struct RegisterValue
    {
        RegisterValue()
        {
            for(std::i64 tobrow = 0; tobrow < hardware::Interposer::TOB_ARRAY_HEIGHT; ++tobrow)
            {
                for (std::i64 tobcol = 0; tobcol < hardware::Interposer::TOB_ARRAY_WIDTH; ++tobcol)
                {
                    tobs.emplace(hardware::TOBCoord{tobrow, tobcol}, TobValue());
                }
            }
            for (std::i64 cobrow = 0; cobrow < hardware::Interposer::COB_ARRAY_HEIGHT; ++cobrow)
            {
                for (std::i64 cobcol = 0; cobcol < hardware::Interposer::COB_ARRAY_WIDTH; ++cobcol)
                {
                    cobs.emplace(hardware::COBCoord{cobrow, cobcol}, CobValue());
                }
            }
        }

        std::HashMap<hardware::TOBCoord, TobValue> tobs;
        std::HashMap<hardware::COBCoord, CobValue> cobs;
    };
}




