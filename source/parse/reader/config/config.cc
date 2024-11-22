
#include "./config.hh"
#include "./interposer.hh"
#include "./topdie.hh"
#include "./externalport.hh"
#include "./connection.hh"

#include <std/collection.hh>
#include <std/file.hh>
#include <debug/debug.hh>
#include <utility/string.hh>
#include <serde/de.hh>
#include <serde/json/json.hh>
#include <xlnt/xlnt.hpp>

namespace kiwi::parse {

    struct ConfigFilepaths {
        std::FilePath interposer;
        std::FilePath topdies;
        std::FilePath topdie_insts;
        std::FilePath external_ports;
        std::FilePath connections;
    };

}

DESERIALIZE_STRUCT(kiwi::parse::ConfigFilepaths,
    DE_FILED(interposer)
    DE_FILED(topdies)
    DE_FILED(topdie_insts)
    DE_FILED(external_ports)
    DE_FILED(connections)
)

template <class ConnectionConfig>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::HashMap<int, std::Vector<ConnectionConfig>>>{
    static void from(const Json& json, std::HashMap<int, std::Vector<ConnectionConfig>>& value){
        auto& map = json.as_object();
        for (auto& [key, j]: map){
            std::Vector<std::Vector<std::String>> vec {};
            kiwi::serde::Deserialize<kiwi::serde::Json, std::Vector<std::Vector<std::String>>>::from(j, vec);

            std::Vector<ConnectionConfig> nets {};
            for (auto& net: vec)
            {
                assert (net.size() == 2);
                nets.emplace_back(ConnectionConfig{net[0], net[1]});
            }
            value.emplace(std::stoi(key), nets);
        }
    }
};

namespace kiwi::parse {

    static auto load_interposer_config(const std::FilePath& path, InterposerConfig& config) -> void;
    static auto load_topdies_config(const std::FilePath& path, std::HashMap<std::String, TopDieConfig>& topdies) -> void;
    static auto load_topdie_insts_config(const std::FilePath& path, std::HashMap<std::String, TopdieInstConfig>& topdie_insts) -> void;
    static auto load_external_ports_config(const std::FilePath& path, std::HashMap<std::String, ExternalPortConfig>& exports) -> void;
    static auto load_connections_config(const std::FilePath& path, std::HashMap<int, std::Vector<ConnectionConfig>>& connections) -> void;

    auto load_config(const std::FilePath& config_folder) -> Config 
    try {
        debug::debug("Load config");
    
        auto config_paths = ConfigFilepaths{};
        serde::deserialize(serde::Json::load_from(config_folder / "config.json"), config_paths);    // deserialize and store in config_paths

        auto config = Config {};

        load_interposer_config(config_folder / config_paths.interposer, config.interposer);
        load_topdies_config(config_folder / config_paths.topdies, config.topdies);
        load_topdie_insts_config(config_folder / config_paths.topdie_insts, config.topdie_insts);
        load_external_ports_config(config_folder / config_paths.external_ports, config.external_ports);
        load_connections_config(config_folder / config_paths.connections, config.connections);

        return config;
    } 
    THROW_UP_WITH("Load config")

    static auto load_interposer_config(const std::FilePath& path, InterposerConfig& config) -> void {
        debug::debug("Load interposer config");

        // Only support .json
        serde::deserialize(serde::Json::load_from(path), config);   
    }

    static auto load_topdies_config(const std::FilePath& path, std::HashMap<std::String, TopDieConfig>& topdies) -> void 
    try {
        debug::debug("Load topdies config");

        auto extension = path.filename().extension().string();
        if (extension == ".json") {
            serde::deserialize(serde::Json::load_from(path), topdies);
        } else if (extension == ".xlsx") {
            debug::unimplement("Load topdie excel");
        } else {
            debug::exception_fmt("Unspport extension '{}' for topdies config", extension);
        }
    } 
    THROW_UP_WITH("Load topdies config")

    static auto load_topdie_insts_config(const std::FilePath& path, std::HashMap<std::String, TopdieInstConfig>& topdie_insts) -> void 
    try {
        debug::debug("Load topdir insts config");

        auto extension = path.filename().extension().string();
        if (extension == ".json") {
            serde::deserialize(serde::Json::load_from(path), topdie_insts);
        } else if (extension == ".xlsx") {
            debug::unimplement("Load topdie insts config by excel");
        } else {
            debug::exception_fmt("Unspport extension '{}' for topdie insts config", extension);
        }
    } 
    THROW_UP_WITH("Load topdie insts config")

    static auto load_external_ports_config(const std::FilePath& path, std::HashMap<std::String, ExternalPortConfig>& exports) -> void 
    try {
        debug::debug("Load external ports config");
        
        auto extension = path.filename().extension().string();
        if (extension == ".json") {
            serde::deserialize(serde::Json::load_from(path), exports);
        } else if (extension == ".xlsx") {
            debug::unimplement("Load external ports excel");
        } else {
            debug::exception_fmt("Unspport extension '{}' for external ports config", extension);
        }
    }
    THROW_UP_WITH("Load external ports config")

    static auto load_connections_config(const std::FilePath& path, std::HashMap<int, std::Vector<ConnectionConfig>>& connections) -> void 
    try {
        debug::debug("Load connections config");

        auto extension = path.filename().extension().string();
        if (extension == ".json") {
            serde::deserialize(serde::Json::load_from(path), connections);
        } 
        else if (extension == ".xlsx") {
            auto workbook = xlnt::workbook();
            workbook.load(path);
            for (const auto& row : workbook.sheet_by_index(0).rows()) {
                // | input  |  output  | syns |
                if (row.length() != 3) {
                    debug::exception_fmt("Except '3' columns but got '{}'", row.length());
                }

                auto input = row[0].to_string();
                if (input.empty()) {
                    continue;
                }
                auto output = row[1].to_string();
                auto sync = utility::string_to_i32(row[2].to_string());

                auto result = connections.emplace(sync, std::Vector<ConnectionConfig>{});
                auto& iter = result.first;
                assert(iter->first == sync);
                auto& v = iter->second.emplace_back(std::move(input), std::move(output));
            }
        } else {
            debug::exception_fmt("Unspport extension '{}' for connections config", extension);
        }
    } 
    THROW_UP_WITH("Load connections config")

}