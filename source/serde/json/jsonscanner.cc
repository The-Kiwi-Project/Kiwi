#include "./jsonscanner.hh"

#include <debug/debug.hh>
#include <std/format.hh>

namespace kiwi::serde {

    std::List<JsonToken> JsonScanner::scan() {
        while (this->is_end() == false) 
            this->scan_token();
        this->add_token(JsonTokenType::ENDOFFILE);

        return std::move(this->_tokens);
    }

    void JsonScanner::scan_token() {
        this->_start_index = this->_current_index;
        
        char ch = this->advance_char(); 
        switch (ch) {
        // Empty character
        case ' ':
        case '\0':
        case '\t':
        case '\n':
        case '\r':
            break;
        case '{':
            this->add_token(JsonTokenType::LEFT_BRACE); break;
        case '}':
            this->add_token(JsonTokenType::RIGHT_BRACE); break;
        case '[':
            this->add_token(JsonTokenType::LEFT_BRACKET); break;
        case ']':
            this->add_token(JsonTokenType::RIGHT_BRACKET); break;
        case ',':
            this->add_token(JsonTokenType::COMMA); break;
        case ':':
            this->add_token(JsonTokenType::COLON); break;
        case 'n':
            this->scan_null_token(); break;
        case 't':
            this->scan_true_token(); break;
        case 'f':
            this->scan_false_token(); break;
        case '0': 
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            this->scan_mumber_token(); break;
        case '"':
            this->scan_string_token(); break;
        default:
            debug::exception_in_fmt("Scan Json", "Unkown character '{}'.", ch);
            break;
        }
    }

    void JsonScanner::scan_mumber_token() {
        // Search until '.' or not digit
        while (true) {
            char ch = this->advance_char();
            if (ch >= '0' && ch <= '9')
                continue;
            
            if (ch != '.') {
                this->_current_index -= 1;

                // Number is a integer
                int value = std::atoi( this->start_pointer() );
                return this->add_token(value);
            }
            else break;
        }

        // Search until not digit
        while (true) {
            char ch = this->advance_char();
            if (ch >= '0' && ch <= '9')
                continue;
            this->_current_index -= 1;

            double value = std::atof( this->start_pointer() );
            return this->add_token(value);
        }
    }

    void JsonScanner::scan_string_token() {
        // Search util '"'
        while ( this->advance_char() != '"') {}

        this->_start_index += 1;
        std::String value(this->start_pointer(), 
                          this->_current_index - this->_start_index - 1);
        this->add_token(std::move(value));
    }

    void JsonScanner::scan_null_token() {
        if (this->advance_char() != 'u' || this->advance_char() != 'l' || this->advance_char() != 'l')
            debug::exception_in("Scan json", "Except 'null'");
        this->add_token(JsonTokenType::NONE);
    }

    void JsonScanner::scan_true_token() {
        if (this->advance_char() != 'r' || this->advance_char() != 'u' || this->advance_char() != 'e')
            debug::exception_in("Scan json", "Except 'true'");
        this->add_token(true);
    }

    void JsonScanner::scan_false_token() {
        if (this->advance_char() != 'a' || this->advance_char() != 'l' || this->advance_char() != 's' || this->advance_char() != 'e')
            debug::exception_in("Scan json", "Except 'false'");
        this->add_token(false);
    }

    char JsonScanner::advance_char() {
        if (this->is_end())
            debug::exception_in("Scan json", "File ended prematurely");
        char ch = this->peek_char();
        this->_current_index += 1;
        return ch;
    }
}