#include "./module.hh"
#include "./config/config.hh"
#include "./reader.hh"
#include "circuit/basedie.hh"
#include "hardware/interposer.hh"

#include <debug/debug.hh>
#include <memory>

namespace kiwi::parse {

    auto read_config(const std::FilePath& config_folder)
        -> std::Tuple<std::Box<hardware::Interposer>, std::Box<circuit::BaseDie>> 
    {
        debug::info_fmt("Read config in '{}'", config_folder.string());

        auto interposer = std::make_unique<hardware::Interposer>();
        auto basedie = std::make_unique<circuit::BaseDie>();

        read_config(config_folder, interposer.get(), basedie.get());
        
        return {std::move(interposer), std::move(basedie)};
    }

    auto read_config(
        const std::FilePath& config_folder,
        hardware::Interposer* interposer,
        circuit::BaseDie* basedie) -> void
    {
        auto config = load_config(config_folder);
        auto reader = Reader{config, interposer, basedie};
        reader.build();
    }

}