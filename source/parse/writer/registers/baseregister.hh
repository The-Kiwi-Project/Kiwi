#pragma once


#include <std/collection.hh>
#include <std/file.hh>

#include "./registervalues.hh"


namespace kiwi::hardware
{
    class Interposer;
}


namespace kiwi::parse
{
    struct RegisterValue;

    class BaseRegister
    {
    public:
        virtual ~BaseRegister() noexcept {}

    public:
        virtual auto fetch_controlbits(RegisterValue& rv) -> void = 0;
        
    };

}

