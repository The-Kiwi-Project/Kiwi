#pragma once

#include "./interposer.hh"
#include "./topdie.hh"
#include "./externalport.hh"
#include "./connection.hh"

#include <std/collection.hh>
#include <std/string.hh>
#include <std/file.hh>

namespace kiwi::parse {

    struct Config {
        InterposerConfig interposer;
        std::HashMap<std::String, TopDieConfig> topdies;
        std::HashMap<std::String, TopdieInstConfig> topdie_insts;
        std::HashMap<std::String, ExternalPortConfig> external_ports;
        std::HashMap<int, std::Vector<ConnectionConfig>> connections;
    };

    auto load_config(const std::FilePath& config_folder) -> Config;

}
