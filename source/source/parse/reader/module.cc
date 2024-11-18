#include "./module.hh"
#include "./config/config.hh"
#include "./reader.hh"

#include <debug/debug.hh>

namespace kiwi::parse {

    auto read_config(const std::FilePath& config_folder)
        -> std::Tuple<std::Box<hardware::Interposer>, std::Box<circuit::BaseDie>> 
    {
        debug::info_fmt("Read config in '{}'", config_folder.string());
        auto config = load_config(config_folder);
        return Reader{config}.build();
    }

}