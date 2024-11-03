#pragma once

#include <std/string.hh>
#include <std/format.hh>

namespace kiwi::console {

    enum class Color {
        Black,
        Red,
        Green,
        Yellow,
        Blue,
        Purple,
        Cyan,
        White,
    };

    auto print_with_color(std::StringView message, Color color) -> void;
    auto print(std::StringView message) -> void;

    auto println_with_color(std::StringView message, Color color) -> void;
    auto println(std::StringView message) -> void;

    auto debug(std::StringView message) -> void;
    auto info(std::StringView message) -> void;
    auto warning(std::StringView message) -> void;
    auto error(std::StringView message) -> void;
    auto fatal(std::StringView message) -> void;

    template <typename... Args>
    auto print_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        print(str);
    }

    template <typename... Args>
    auto println_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        println(str);
    }

    template <typename... Args>
    auto debug_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        debug(str);
    }

    template <typename... Args>
    auto info_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        info(str);
    }

    template <typename... Args>
    auto warning_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        warning(str);
    }

    template <typename... Args>
    auto error_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        error(str);
    }

    template <typename... Args>
    auto fatal_fmt(std::FormatString<Args...> fmt, Args &&... args) -> void {
        auto str = std::format(std::move(fmt), std::forward<Args>(args)...);
        fatal(str);
    }

}