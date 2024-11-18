#pragma once

#include <std/string.hh>
#include <std/integer.hh>

namespace kiwi::serde {

    enum class JsonTokenType {
        // []
        LEFT_BRACKET, RIGHT_BRACKET,
        // {}
        LEFT_BRACE, RIGHT_BRACE,
        // , :
        COMMA, COLON,
        // "hello" 12 89.332 trur null
        STRING, INTEGER, DECIMAL, BOOLEAN, NONE,
        // END OF FILE
        ENDOFFILE,
    };

    class JsonToken {
    public:
        JsonToken(JsonTokenType type) noexcept:
            _type{type}, _value{0} {}
        
        JsonToken(bool value) noexcept: 
            _type{JsonTokenType::BOOLEAN}, _bool{value} {}
        
        JsonToken(int value) noexcept: 
            _type{JsonTokenType::INTEGER}, _int{value} {}

        JsonToken(double value) noexcept: 
            _type{JsonTokenType::DECIMAL}, _double{value} {}

        JsonToken(std::string value) noexcept: 
            _type{JsonTokenType::STRING}, _string{std::move(value)} {}

        ~JsonToken() noexcept {}

    public:
        bool boolean() const noexcept
        { return this->_bool; }

        int integer() const noexcept
        { return this->_int; }

        double decimal() const noexcept
        { return this->_double; }

        std::string& string() noexcept
        { return this->_string; }

        JsonTokenType type() const noexcept
        { return this->_type; }

    private:
        JsonTokenType _type;

        union {
            bool _bool;
            int _int;
            double _double;
            std::string _string;
            std::uint64_t _value;
        };
    };

}