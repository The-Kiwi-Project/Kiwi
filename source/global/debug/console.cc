#include "console.hh"
#include <iostream>

namespace kiwi::console {

    static void print_level(Color color, std::StringView level)
    {
        std::cout << '[';
        print_with_color(level, color);
        std::cout << "]: "; 
    }

    void print_with_color(std::StringView message, Color color) {
        const char* colorInfo = nullptr;
        switch (color) {
            case Color::Black:  colorInfo = "\033[30m"; break;
            case Color::Red:    colorInfo = "\033[31m"; break;
            case Color::Green:  colorInfo = "\033[32m"; break;
            case Color::Yellow: colorInfo = "\033[33m"; break;
            case Color::Blue:   colorInfo = "\033[34m"; break;
            case Color::Purple: colorInfo = "\033[35m"; break;
            case Color::Cyan:   colorInfo = "\033[36m"; break;
            case Color::White:  colorInfo = "\033[37m"; break;
        }
        std::cout << colorInfo << message << "\033[0m" << std::ends;
    }

    void print(std::StringView message) {
        std::cout << message << std::ends;
    }

    void println_with_color(std::StringView message, Color color) {
        print_with_color(message, color);
        std::cout << std::endl;
    }

    void println(std::StringView message) {
        std::cout << message << std::endl;
    }

    void debug(std::StringView message) {
        print_level(Color::Blue, " DEBUG ");
        println(message);
    }

    void info(std::StringView message) {
        print_level(Color::Green, " INFO  ");
        println(message);
    }

    void warning(std::StringView message) {
        print_level(Color::Yellow, "WARNING");
        println(message);
    }

    void error(std::StringView message) {
        print_level(Color::Red, " ERROR ");
        println(message);
    }

    void fatal(std::StringView message) {
        print_level(Color::Red, " FATAL ");
        println(message);
    }

}