#pragma once

#include "./jsontype.hh"
#include <exception>
#include <string>

namespace kiwi::serde 
{

    class JsonKeyError : public std::exception {
    public:
        JsonKeyError(std::string_view key);
        virtual const char* what() const noexcept override;
        
    private:
        std::string _msg;
    };

    class JsonTypeError : public std::exception {
    public:
        JsonTypeError(JsonType except, JsonType got);
        virtual const char* what() const noexcept override;

    private:
        std::string _msg;
    };

    class JsonIndexError : public std::exception {
    public:
        JsonIndexError(std::size_t except, std::size_t max);
        virtual const char* what() const noexcept override;

    private:
        std::string _msg;
    };

}