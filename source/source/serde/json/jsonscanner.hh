#pragma once 

#include "./jsontoken.hh"

#include <std/string.hh>
#include <std/collection.hh>

namespace kiwi::serde {

    class JsonScanner {
    public:
        JsonScanner(std::String content) noexcept:
            _content{std::move(content)},
            _start_index{0},
            _current_index{0} {}

        std::List<JsonToken> scan();

    private:
        void scan_token();

        void scan_mumber_token();
        void scan_string_token();
        void scan_null_token();
        void scan_true_token();
        void scan_false_token();
    
    private:

        char advance_char();
        
        char peek_char() noexcept
        { return this->_content[this->_current_index]; }

        bool is_end() const noexcept
        { return this->_current_index >= this->_content.size(); }

        const char* start_pointer() const noexcept
        { return &(this->_content[this->_start_index]); }

    private:
        void add_token(JsonTokenType type) 
        { this->_tokens.emplace_back(type); }

        void add_token(bool value)
        { this->_tokens.emplace_back(value); }

        void add_token(int value)
        { this->_tokens.emplace_back(value); }

        void add_token(double value)
        { this->_tokens.emplace_back(value); }

        void add_token(std::String value)
        { this->_tokens.emplace_back(std::move(std::move(value))); } 

    private:
        std::String _content;
        int _start_index;
        int _current_index;                 // point to the element following the current one
        std::List<JsonToken> _tokens;
    };

}