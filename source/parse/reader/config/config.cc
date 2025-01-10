
#include "./config.hh"
#include "./interposer.hh"
#include "./topdie.hh"
#include "./externalport.hh"
#include "./connection.hh"

#include <hardware/interposer.hh>
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
        std::FilePath ports_01;
    };

}

DESERIALIZE_STRUCT(kiwi::parse::ConfigFilepaths,
    DE_FILED(interposer)
    DE_FILED(topdies)
    DE_FILED(topdie_insts)
    DE_FILED(external_ports)
    DE_FILED(connections)
    DE_FILED(ports_01)
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
    static auto load_ports_01_config(const std::FilePath& path, std::HashMap<std::String, std::HashMap<std::String, hardware::TrackCoord>>& ports_01) -> void;

    static auto load_from_txt(const std::FilePath& path, Config& config) -> void;
    static auto parse_txt_line(const std::String& topdie1, const std::String& topdie2, const std::Array<int, 11>& numbers, Config& config) -> void;

    auto load_config(const std::FilePath& config_folder) -> Config 
    try {
        debug::debug("Load config");
    
        auto config_paths = ConfigFilepaths{};
        serde::deserialize(serde::Json::load_from(config_folder / "config.json"), config_paths);    // deserialize and store in config_paths

        auto config = Config {};

        load_ports_01_config(config_folder / config_paths.ports_01, config.ports_01);
        if (config_paths.connections.filename().extension().string() == ".json"){
            load_interposer_config(config_folder / config_paths.interposer, config.interposer);
            load_topdies_config(config_folder / config_paths.topdies, config.topdies);
            load_topdie_insts_config(config_folder / config_paths.topdie_insts, config.topdie_insts);
            load_external_ports_config(config_folder / config_paths.external_ports, config.external_ports);
            load_connections_config(config_folder / config_paths.connections, config.connections);
        }
        else if(config_paths.connections.filename().extension().string() == ".txt"){
            load_from_txt(config_folder / config_paths.connections, config);
        }
        else{
            debug::exception_fmt("Unspport extension '{}' for connections config", config_paths.connections.filename().extension().string());
        }

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

    static auto load_from_txt(const std::FilePath& path, Config& config) -> void 
    try {
        debug::debug("Load from txt");

        std::ifstream file(path);
        if (!file.is_open()){
            debug::exception_fmt("Cannot open file '{}'", path.string());
        }

        std::Array<int, 11> numbers;  // input[dir, x, y, bump_x, bump_y], output[dir, x, y, bump_x, bump_y], net_tag
        std::string line, topdie_name1, topdie_name2;
        while (std::getline(file, line)) {
            // skip empty line
            if (line.find_first_not_of(" \t") == std::string::npos) {
                continue;  
            }

            if (line[0] == '#') {
                std::istringstream iss(line.substr(1));  
                if (iss >> topdie_name1 >> topdie_name2) {  
                    ;
                } 
                else {
                    throw std::runtime_error(std::format("Invalie line: '{}'", line));
                }

                continue;  
            }

            // parse line
            std::stringstream ss(line);  
            int num, pos=0;
            while (ss >> num) {
                numbers[pos++] = num;
            }
            
            parse_txt_line(topdie_name1, topdie_name2, numbers, config);
        }

       file.close();
    }
    THROW_UP_WITH("Load from txt")

    auto parse_txt_line(const std::String& topdie1, const std::String& topdie2, const std::Array<int, 11>& numbers, Config& config) -> void
    try{
        const auto net_tag = numbers.back();
        std::String input{}, output{};

        auto parse_node = [&config](const std::Array<int, 5>& info, std::String& node, const std::String& topdie){
            std::Vector<int> externs { 0,9,18,27,36,45,54,63,120,113,106,99,92,85,78,71 };
            if (topdie != "cpu" && topdie != "AI" && topdie != "mem" && topdie != "extIO" && topdie != "0/1"){
                debug::exception_fmt("Invalid topdie_name '{}'", topdie);
            }
            switch (info[3]){
                case -1: node = "nege"; 
                    break;
                case -2: node = "pose";
                    break;
                case -3: {
                    auto trackcoord = info[0] == 0?\
                        hardware::TrackCoord(hardware::Interposer::COB_ARRAY_HEIGHT-info[1], info[2], hardware::TrackDirection::Vertical, externs[info[4]]):\
                        hardware::TrackCoord(hardware::Interposer::COB_ARRAY_HEIGHT-info[1], info[2], hardware::TrackDirection::Horizontal, externs[info[4]]);
                    node = std::String{
                        "IO_"+std::to_string(info[0])+"_"+std::to_string(info[1])+\
                        "_"+std::to_string(info[2])+"_"+std::to_string(info[4])
                    };
                    if (!config.external_ports.contains(node)){
                        config.external_ports.emplace(node, ExternalPortConfig{trackcoord});
                    }
                    break;
                }
                default:{
                    auto index = info[3] + 8 * info[4];
                    auto node_postfix = std::String{std::to_string(info[1]) + "_" + std::to_string(info[2]) + "_" + std::to_string(index)};
                    if(!config.topdies.contains(topdie)){
                        config.topdies.emplace(topdie, TopDieConfig{});
                    }
                    if (!config.topdies.at(topdie).pin_map.contains(node_postfix)){
                        config.topdies.at(topdie).pin_map.emplace(node_postfix, index);
                    }

                    std::String topdie_inst {
                        "Topdie_inst_" + std::to_string(info[1]) + "_" + std::to_string(info[2])
                    };
                    node = topdie_inst + "." + node_postfix;
                    auto tobcoord = hardware::TOBCoord((hardware::Interposer::COB_ARRAY_HEIGHT-info[1])/2, info[2]/3);
                    if (!config.topdie_insts.contains(topdie_inst)){
                        config.topdie_insts.emplace(topdie_inst, TopdieInstConfig{topdie, tobcoord});
                    }
                    break;
                }
            }
        };

        parse_node({numbers[0], numbers[1], numbers[2], numbers[3], numbers[4]}, input, topdie1);
        parse_node({numbers[5], numbers[6], numbers[7], numbers[8], numbers[9]}, output, topdie2);
        if (!config.connections.contains(net_tag)){
            config.connections.emplace(net_tag, std::Vector<ConnectionConfig>{});
        }
        config.connections.at(net_tag).emplace_back(ConnectionConfig{input, output});
    }
    THROW_UP_WITH("Parse txt line")

    auto load_ports_01_config(const std::FilePath& path, std::HashMap<std::String, std::HashMap<std::String, hardware::TrackCoord>>& ports_01) -> void
    try {
        debug::debug("Load 0/1 ports config");
        
        auto extension = path.filename().extension().string();
        if (extension == ".json") {
            serde::deserialize(serde::Json::load_from(path), ports_01);
        } else if (extension == ".xlsx") {
            debug::unimplement("Load 0/1 ports excel");
        } else {
            debug::exception_fmt("Unspport extension '{}' for 0/1 ports config", extension);
        }
    }
    THROW_UP_WITH("Load 0/1 ports config")

}