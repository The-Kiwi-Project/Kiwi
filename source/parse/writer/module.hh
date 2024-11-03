#pragma once

#include "std/file.hh"

namespace kiwi::hardware {
    class Interposer;
}

namespace kiwi::parse {

    auto write_control_bits(hardware::Interposer* interposer, const std::FilePath& output_path) -> void;

}