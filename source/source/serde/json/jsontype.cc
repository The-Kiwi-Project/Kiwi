#include "./jsontype.hh"
#include <std/string.hh>

namespace kiwi::serde {

    auto jsonTypeToString(JsonType type) -> std::StringView {
        switch (type) {
            case JsonType::Null: return "null";
            case JsonType::Boolean: return "boolean";
            case JsonType::Integer: return "integer";
            case JsonType::Decimal: return "decimal";
            case JsonType::String: return "string";
            case JsonType::Array: return "array";
            case JsonType::Object: return "object";
        }
        return "";
    }

}