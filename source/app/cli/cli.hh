#pragma once

#include "std/string.hh"
#include "std/utility.hh"

namespace kiwi {

    auto cli_main(std::StringView config_path, std::Option<std::StringView> output_path) -> void;

}