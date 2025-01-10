#pragma once

#include "std/file.hh"
#include <hardware/interposer.hh>
#include <circuit/basedie.hh>
#include <std/utility.hh>

#include <hardware/interposer.hh>

namespace kiwi::parse {

    auto read_config(const std::FilePath& config_folder) 
        -> std::Tuple<std::Box<hardware::Interposer>, std::Box<circuit::BaseDie>>;

}