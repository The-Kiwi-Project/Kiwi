#include "./file.hh"
#include <filesystem>
#include <format>
#include <std/file.hh>
#include <std/string.hh>

namespace kiwi::utility {

    std::size_t filesize(std::ifstream& file) {
        file.seekg(0, std::ios::end);
        std::size_t size{ static_cast<std::size_t>(file.tellg()) };
        file.seekg(0, std::ios::beg);
        return size;
    }

    std::string read_file(std::InFile& file) {
        std::size_t size {filesize(file)};
        std::string content {};
        content.resize(size + 1);
        file.read(content.data(), size);
        content.resize(size);
        return content;
    }

    std::string read_file(const std::filesystem::path& filepath) {
        auto infile = std::InFile{filepath};
        if (!infile.is_open()) 
            throw std::runtime_error {std::format("util::read_file: File '{}' does not open.", filepath.string())};
        auto content = read_file(infile);
        infile.close();
        return content;
    }

}