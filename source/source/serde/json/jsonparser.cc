#include "./jsonparser.hh"
#include "./json.hh"

#include <debug/debug.hh>

namespace kiwi::serde
{

    Json JsonParser::parse()  {
        JsonToken& token = this->advanceToken();
     
        switch (token.type()) {
        case JsonTokenType::LEFT_BRACE:
            return this->parseObject();
            break;
        case JsonTokenType::LEFT_BRACKET:
            return this->parseArray();
            break;
        case JsonTokenType::STRING:
            return Json::string(std::move(token.string()));
            break;
        case JsonTokenType::NONE:
            return Json{};
            break;
        case JsonTokenType::BOOLEAN:
            return Json::boolean(token.boolean());
            break;
        case JsonTokenType::INTEGER:
            return Json::integer(token.integer());
            break;
        case JsonTokenType::DECIMAL:
            return Json::decimal(token.decimal());
            break;
        default:
            debug::exception_in("Parse json", "Not an valid json value");
            break;
        }
        return Json{};
    }

    Json JsonParser::parseObject()
    {
        auto json = Json::object();

        // First "key" : value 
        if (this->checkTokenType(JsonTokenType::STRING))
        {
            // Get Key
            JsonToken& keyToken = this->advanceToken();
            this->ensureTokenType(JsonTokenType::COLON, "Except ':'");
            json.insert(std::move(keyToken.string()), this->parse());
        }

        // "key" : value paris
        while (true)
        {
            if (this->checkTokenType(JsonTokenType::COMMA) == false)
                break;
            this->jumpToken(); 

            // Must be string
            JsonToken& keyToken = this->ensureTokenType(JsonTokenType::STRING, "Except a string as key");
            this->ensureTokenType(JsonTokenType::COLON, "Except ':'");
            json.insert(std::move(keyToken.string()), this->parse());
        }

        this->ensureTokenType(JsonTokenType::RIGHT_BRACE, "Except '}'");
        return json;
    }

    Json JsonParser::parseArray()
    {
        auto json = Json::array();

        // First json value
        if (this->checkTokenType(JsonTokenType::RIGHT_BRACKET) == false)
            json.push(this->parse());
        
        // Json values
        while (true)
        {
            if (this->checkTokenType(JsonTokenType::COMMA) == false)
                break;
            this->jumpToken();

            json.push(this->parse());
        }

        this->ensureTokenType(JsonTokenType::RIGHT_BRACKET, "Except ']'");
        return json;
    }

    bool JsonParser::checkTokenType(JsonTokenType type)
    {
        if (this->isEnd())
            debug::exception_in("Parse json", "Tokens ended prematurely");
        return this->_iterator->type() == type;
    }

    JsonToken& JsonParser::ensureTokenType(JsonTokenType type, const char* message)
    {
        if (this->isEnd())
            debug::exception_in("Parse json", "Tokens ended prematurely");
        
        if (this->_iterator->type() != type)
            debug::exception_in("Parse json", message);
        return *(this->_iterator++);
    }

    JsonToken& JsonParser::advanceToken()
    {
        if (this->isEnd())
            debug::exception_in("Parse json", "Tokens ended prematurely");
        return *(this->_iterator++);
    }
}
