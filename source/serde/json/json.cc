#include "./json.hh"
#include "./jsonscanner.hh"
#include "./jsonparser.hh"
#include "./jsonerror.hh"

#include <std/file.hh>
#include <std/collection.hh>
#include <std/collection.hh>

#include <utility/file.hh>

namespace kiwi::serde {

    static constexpr auto json_type_to_integer(JsonType type) -> std::usize {
        return static_cast<std::usize>(type);
    }

    static constexpr auto integer_to_json_type(std::usize i) -> JsonType {
        return static_cast<JsonType>(i);
    }

    Json::Json() :
        _value{Null{}} 
    {
    }

    auto Json::null() -> Json {
        return Json{};
    }

    auto Json::boolean(bool value) -> Json {
        auto j = Json{};
        j._value = value;
        return j;
    }

    auto Json::integer(int value) -> Json {
        auto j = Json{};
        j._value = value;
        return j;
    }

    auto Json::decimal(double value) -> Json {
        auto j = Json{};
        j._value = value;
        return j;
    }

    auto Json::string(std::String value) -> Json {
        auto j = Json{};
        j._value = value;
        return j;
    }

    auto Json::array() -> Json {
        auto j = Json{};
        j._value = std::Vector<Json>{};
        return j;
    }

    auto Json::object() -> Json {
        auto j = Json{};
        j._value = std::HashMap<std::String, Json>{};
        return j;
    }

    bool Json::is_null() const noexcept {
        return this->check_type(JsonType::Null);
    }

    bool Json::is_boolean() const noexcept {
        return this->check_type(JsonType::Boolean);
    }

    bool Json::is_integer() const noexcept {
        return this->check_type(JsonType::Integer);
    }

    bool Json::is_decimal() const noexcept {
        return this->check_type(JsonType::Decimal);
    }

    bool Json::is_string() const noexcept {
        return this->check_type(JsonType::String);
    }

    bool Json::is_array() const noexcept {
        return this->check_type(JsonType::Array);
    }

    bool Json::is_object() const noexcept {
        return this->check_type(JsonType::Object);
    }

    ///////////////////////////////////////////////////////////////////////////

    auto Json::as_boolean() const -> bool {   
        this->ensure_tyep(JsonType::Boolean);
        return std::get<json_type_to_integer(JsonType::Boolean)>(this->_value);
    }

    auto Json::as_integer() const -> int {   
        this->ensure_tyep(JsonType::Integer);
        return std::get<json_type_to_integer(JsonType::Integer)>(this->_value);
    }

    auto Json::as_decimal() const -> double {   
        this->ensure_tyep(JsonType::Decimal);
        return std::get<json_type_to_integer(JsonType::Decimal)>(this->_value);
    }

    auto Json::as_string() const -> std::StringView {   
        this->ensure_tyep(JsonType::String);
        return std::get<json_type_to_integer(JsonType::String)>(this->_value);
    }

    auto Json::as_array() const -> std::span<const Json> {   
        this->ensure_tyep(JsonType::Array);
        return std::get<json_type_to_integer(JsonType::Array)>(this->_value);
    }

    auto Json::as_object() const -> const std::HashMap<std::String, Json>& {   
        this->ensure_tyep(JsonType::Object);
        return std::get<json_type_to_integer(JsonType::Object)>(this->_value);
    }

    ///////////////////////////////////////////////////////////////////////////

    auto Json::as_boolean() -> bool& {   
        this->ensure_tyep(JsonType::Boolean);
        return std::get<static_cast<std::usize>(JsonType::Boolean)>(this->_value);
    }

    auto Json::as_integer() -> int& {   
        this->ensure_tyep(JsonType::Integer);
        return std::get<static_cast<std::usize>(JsonType::Integer)>(this->_value);
    }

    auto Json::as_decimal() -> double& {   
        this->ensure_tyep(JsonType::Decimal);
        return std::get<static_cast<std::usize>(JsonType::Decimal)>(this->_value);
    }

    auto Json::as_string() -> std::String& {   
        this->ensure_tyep(JsonType::String);
        return std::get<static_cast<std::usize>(JsonType::String)>(this->_value);
    }

    auto Json::as_array() -> std::Vector<Json>& {   
        this->ensure_tyep(JsonType::Array);
        return std::get<static_cast<std::usize>(JsonType::Array)>(this->_value);
    }

    auto Json::as_object() -> std::HashMap<std::String, Json>& {   
        this->ensure_tyep(JsonType::Object);
        return std::get<static_cast<std::usize>(JsonType::Object)>(this->_value);
    }

    ///////////////////////////////////////////////////////////////////////////

    auto Json::at(const std::String& key) const -> const Json& {
        auto& map = this->as_object();
        auto res = map.find(key);
        if (res == map.end()) 
            throw JsonKeyError{key};
        return res->second;
    }

    auto Json::at(const std::String& key) -> Json& {
        auto& map = this->as_object();
        auto res = map.emplace(key, Json{});
        return res.first->second;
    }

    auto Json::operator [] (const std::String& key) const  -> const Json& {
        return this->at(key);
    }

    auto Json::operator [] (const std::String& key) -> const Json& {
        return this->at(key);
    }
    
    auto Json::get(const std::String& key) const -> std::optional<const Json*>  {
        auto& map = this->as_object();
        auto res = map.find(key);
        if (res == map.end()) 
            return std::nullopt;
        return std::optional<const Json*> {&(res->second)};
    }

    auto Json::get(const std::String& key) -> std::optional<Json*> {
        auto& map = this->as_object();
        auto res = map.find(key);
        if (res == map.end()) 
            return std::nullopt;
        return std::optional<Json*>{&(res->second)};
    }

    auto Json::contains(const std::String& key) const -> bool {
        auto& map = this->as_object();
        return map.contains(key);
    }

    auto Json::size() const -> std::usize {
        return this->as_object().size();
    }

    auto Json::insert(std::String key, Json value) -> void {
        auto& map = this->as_object();
        map.emplace(std::move(key), std::move(value));
    }

    /////////////////////////////////////////////////////////////////////////////

    auto Json::at(std::usize index) const -> const Json& {
        auto arr = this->as_array();
        if (index >= arr.size()) 
            throw JsonIndexError{index, arr.size()};
        return arr[index]; 
    }

    auto Json::at(std::usize index) -> Json& {
        auto& arr = this->as_array();
        if (index >= arr.size()) 
            throw JsonIndexError{arr.size(), index};
        return arr[index]; 
    }

    auto Json::operator [] (std::usize index) const -> const Json& {
        return this->at(index);
    }
    
    auto Json::operator [] (std::usize index) -> Json& {
        return this->at(index);
    }

    auto Json::get(std::usize index) const -> std::optional<const Json*> {
        auto arr = this->as_array();
        if (index >= arr.size()) 
            return std::nullopt;
        return std::optional<const Json*>{&arr[index]}; 
    }

    auto Json::get(std::usize index) -> std::optional<Json*> {
        auto& arr = this->as_array();
        if (index >= arr.size()) 
            return std::nullopt;
        return std::optional<Json*>{&arr[index]}; 
    }

    /////////////////////////////////////////////////////////

    auto Json::len() const -> std::usize {
        return this->as_array().size();
    }

    auto Json::push(Json json) -> void {
        return this->as_array().push_back(std::move(json));
    }

    /////////////////////////////////////////////////////////////////

    auto Json::to_string() const -> std::String {
        return this->do_to_string();
    }

    /////////////////////////////////////////////////////////////////

    void Json::ensure_tyep(JsonType type) const {
        if (!this->check_type(type)) 
            throw JsonTypeError{type, this->type()};
    }

    auto Json::check_type(JsonType type) const -> bool {
        return this->type_index() == static_cast<std::usize>(type);
    }

    auto Json::type_index() const noexcept -> std::usize {
        return this->_value.index();
    }

    auto Json::type() const noexcept -> JsonType {
        return integer_to_json_type(this->_value.index());
    }

    Json Json::load_from(const std::filesystem::path& filepath) {
        auto content = utility::read_file(filepath);
        auto tokens = JsonScanner{std::move(content)}.scan();
        return JsonParser{std::move(tokens)}.parse();
    }

    std::String Json::do_to_string(std::usize indent, bool hasName) const {
        std::stringstream ss;

        auto print_space = [&ss] (std::usize indent) {
            for (std::usize i = 0; i < indent; ++i)
                ss << "    ";
        };

        switch (this->type()) {
            case JsonType::Null:
                ss << "null";
                break;
            case JsonType::Boolean:
                ss << (this->as_boolean() == true ? "true" : "false"); 
                break;
            case JsonType::Integer:
                ss << this->as_integer();
                break;
            case JsonType::Decimal:
                ss << this->as_decimal();
                break;
            case JsonType::String:
                ss << '\"' << this->as_string() << '\"';
                break;
            case JsonType::Array: {
                if (!hasName)  print_space(indent);
                ss << '[' << '\n';

                auto array = this->as_array();
                for (auto iter = array.begin(); iter != array.end(); ++iter) {
                    if (iter != array.begin())
                        ss << ',' << '\n';
                    print_space(indent + 1);
                    ss << iter->do_to_string(indent + 1, true);
                }

                ss << '\n';
                print_space(indent);
                ss << ']';
                break;
            }
            case JsonType::Object: {
                if (!hasName) print_space(indent);
                ss << '{' << '\n';

                auto& map = this->as_object();
                for (auto iter = map.begin(); iter != map.end(); ++iter) {
                    if (iter != map.begin())
                        ss << ',' << '\n';
                    print_space(indent + 1);
                    ss << '\"' << iter->first << '\"' << " : " 
                    << iter->second.do_to_string(indent + 1, true);
                }

                ss << '\n';
                print_space(indent);
                ss << '}';
                break;
            }
            default:
                break;
        }

        return ss.str();
    }

}
