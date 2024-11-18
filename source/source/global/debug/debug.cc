#include "./debug.hh"
#include "std/string.hh"

#include <cstdlib>
#include <debug/console.hh>
#include <debug/log.hh>

#include <std/file.hh>
#include <std/integer.hh>
#include <std/exception.hh>

namespace kiwi::debug {

    static auto debug_level = DebugLevel::Debug;

    static auto debug_level_to_number(DebugLevel level) -> std::i64 {
        switch (level) {
            case DebugLevel::Debug:   return 0;
            case DebugLevel::Info:    return 1;
            case DebugLevel::Warning: return 2;
            case DebugLevel::Error:   return 3;
            case DebugLevel::Fatal:   return 4;
        }
        return -1;
    }

    auto initial_log(const std::FilePath& log_path) -> void {
        log::initial(log_path);
    }

    auto set_debug_level(DebugLevel level) -> void {
        debug_level = level;
    }

    auto is_debug_level_enough(DebugLevel level) -> bool {
        return level >= debug_level;
    }

    auto debug(std::StringView message) -> void {
        console::debug(message);
        log::debug(message);
    }

    auto info(std::StringView message) -> void {
        console::info(message);
        log::info(message);
    }

    auto warning(std::StringView message) -> void {
        console::warning(message);
        log::warning(message);
    }

    auto error(std::StringView message) -> void {
        console::error(message);
        log::error(message);
    }

    [[noreturn]] auto fatal(std::StringView message) -> void {
        console::fatal(message);
        log::fatal(message);
        std::exit(-1);
    }

    auto check(bool condition, std::StringView message) -> void {
        if (!condition) {
            exception(message);
        } 
    }

    [[noreturn]] auto exception(std::StringView message) -> void {
        throw std::RunTimeError{std::String{message}};
    }

    [[noreturn]] auto exception_in(std::StringView where, std::StringView message) -> void {
        throw std::RunTimeError{std::format("{} >> {}", where, message)};
    }

    [[noreturn]] auto unreachable(std::StringView message) -> void {
        if (message.empty()) {
            fatal("Reach unreachable");
        } else {
            fatal_fmt("Reach unreachable in '{}'", message);
        }
    }

    [[noreturn]] auto unimplement(std::StringView message) -> void {
        if (message.empty()) {
            fatal("Reach unreachable");
        } else {
            fatal_fmt("Reach unimplement in '{}'", message);
        }
    }

}