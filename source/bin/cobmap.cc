#include "debug/debug.hh"
#include <hardware/cob/cobdirection.hh>
#include <hardware/cob/cob.hh>
#include <std/integer.hh>
#include <std/utility.hh>
#include <std/string.hh>

using namespace kiwi::hardware;

auto str_to_dir(std::StringView str) -> std::Option<COBDirection> {
    if (str == "left") return { COBDirection::Left };
    if (str == "l") return { COBDirection::Left };
    if (str == "right") return { COBDirection::Right };
    if (str == "r") return { COBDirection::Right };
    if (str == "up") return { COBDirection::Up };
    if (str == "u") return { COBDirection::Up };
    if (str == "down") return { COBDirection::Down };
    if (str == "d") return { COBDirection::Down };
    return std::nullopt;
}

auto ensure_str_to_dir(std::StringView str) -> COBDirection {
    auto opt_dir = str_to_dir(str);
    if (!opt_dir.has_value()) {
        kiwi::debug::fatal_fmt("'{}' is not a valid direction literal!", str);
    }
    return *opt_dir;
}

auto ensure_str_to_usize(std::StringView str) -> std::usize {
    try {
        return std::stoll(str.data());
    } catch (const std::exception& err) {
        kiwi::debug::fatal_fmt("Parse number failed, '{}'", err.what());
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        kiwi::debug::info("Usage: cob_map <cob_index/track_index> <from_dir> <from_index> <to_dir>");
    } else {
        auto command = std::StringView{argv[1]};
        auto from_dir_str = std::StringView{argv[2]};
        auto from_index_str = std::StringView{argv[3]};
        auto to_dir_str = std::StringView{argv[4]};

        auto from_dir = ensure_str_to_dir(from_dir_str);
        auto from_index = ensure_str_to_usize(from_index_str);
        auto to_dir = ensure_str_to_dir(to_dir_str);

        std::usize to_index = 0;
        if (command == "cob_index") {
            to_index = COB::cob_index_map(from_dir, from_index, to_dir);   
        } else if (command == "tracl_index") {
            to_index = COB::track_index_map(from_dir, from_index, to_dir);   
        } else {
            kiwi::debug::fatal_fmt("Unkown command: ", command);
        }

        kiwi::debug::info_fmt("to_index = {}", to_index);

    }

    return 0;
}