#pragma once

#include "std/string.hh"
#include "std/utility.hh"
#include <circuit/net/net.hh>

namespace kiwi {

    auto cli_main(std::StringView config_path, std::Option<std::StringView> output_path) -> int;
    
}