#include "./string.hh"
#include <std/exception.hh>
#include <debug/debug.hh>

namespace kiwi::utility {

    auto string_to_i32(std::StringView s) -> std::i32 
    try {
        return std::stoi(std::String{s});
    }
    catch (const std::Exception& err) {
        debug::exception_fmt("'{}' is not invalid i32 value", s);
    }

    auto split(std::StringView origin, char delim) -> std::Vector<std::StringView> {
        std::Vector<std::StringView> result;
        size_t start = 0;
        size_t end;

        while ((end = origin.find(delim, start)) != std::StringView::npos) {
            result.emplace_back(origin.substr(start, end - start));
            start = end + 1;
        }

        // Add the last segment
        if (start < origin.size()) {
            result.emplace_back(origin.substr(start));
        }

        return result;
    }

}