#include "./jsonerror.hh"
#include "./jsontype.hh"
#include <std/format.hh>

namespace kiwi::serde 
{

    JsonKeyError::JsonKeyError(std::string_view key) :
        _msg{} 
    {
        this->_msg = std::format("Not exits key '{}'", key);
    }
   
    const char* JsonKeyError::what() const noexcept {
        return this->_msg.c_str();
    }

    JsonTypeError::JsonTypeError(JsonType except, JsonType got) :
        _msg{}
    {
        this->_msg = 
            std::format("Except '{}' but got '{}'", jsonTypeToString(except), jsonTypeToString(got));
    }
   
    const char* JsonTypeError::what() const noexcept {
        return this->_msg.c_str();
    }

    JsonIndexError::JsonIndexError(std::size_t except, std::size_t max) :
        _msg{}
    {
        this->_msg = 
            std::format("Except index '{}' but got max size if '{}'", except, max);
    }
   
    const char* JsonIndexError::what() const noexcept {
        return this->_msg.c_str();
    }

}