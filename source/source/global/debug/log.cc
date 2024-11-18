#include "./log.hh"
#include <std/file.hh>

namespace kiwi::log {

    static auto log_file = std::OutFile{};

    static auto log(std::StringView level, std::StringView message) -> void;
    static auto log_date_time() -> void;
    static auto log_level(std::StringView level) -> void;
    static auto log_enable() -> bool;

    auto initial(const std::FilePath& log_path) -> void {
        log_file.open(log_path);
    }

    auto debug(std::StringView message) -> void {
        if (!log_enable()) return;
        log(" DEBUG ", message);
    }

    auto info(std::StringView message) -> void {
        if (!log_enable()) return;
        log(" INFO  ", message);
    }

    auto warning(std::StringView message) -> void {
        if (!log_enable()) return;
        log("WARNING", message);
    }

    auto error(std::StringView message) -> void {
        if (!log_enable()) return;
        log(" ERROR ", message);
    }

    auto fatal(std::StringView message) -> void {
        if (!log_enable()) return;
        log(" FATAL ", message);
    }

    static auto log(std::StringView level, std::StringView message) -> void
    {
        log_date_time();
        log_level(level);
        log_file << message << '\n';
        log_file.flush();
    }

    static auto log_date_time() -> void
    {
        time_t current_ticks = std::time(NULL);
        struct tm* time = std::localtime(&current_ticks);

        char dateTime[32] = {0};
        std::memset(dateTime, 0, sizeof(dateTime));
        std::strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", time);
        log_file << '[' << dateTime << ']';
    }

    static auto log_level(std::StringView level) -> void
    {
        log_file << ' ' << level << " > ";
    }

    static auto log_enable() -> bool {
        return log_file.is_open();
    }

}
