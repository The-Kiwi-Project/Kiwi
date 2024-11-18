#pragma once

#include "../serde.hh"
#include "./jsontype.hh"

#include <std/memory.hh>
#include <std/collection.hh>
#include <std/utility.hh>
#include <std/string.hh>
#include <std/file.hh>
#include <std/integer.hh>

namespace kiwi::serde {
    
    class Json {
    public:
        Json();

    public:
        static auto null() -> Json;
        static auto boolean(bool value) -> Json;
        static auto integer(int value) -> Json;
        static auto decimal(double value) -> Json;
        static auto string(std::String value) -> Json;
        static auto array() -> Json;
        static auto object() -> Json;

        static auto load_from(const std::filesystem::path& filepath) -> Json;

    public:
        bool is_null() const noexcept;
        bool is_boolean() const noexcept;
        bool is_integer() const noexcept;   
        bool is_decimal() const noexcept;
        bool is_string() const noexcept;
        bool is_array() const noexcept;
        bool is_object() const noexcept;

    public:
        auto as_boolean() const -> bool;
        auto as_integer() const -> int;   
        auto as_decimal() const -> double;
        auto as_string() const -> std::StringView;
        auto as_array() const -> std::span<const Json>;
        auto as_object() const -> const std::HashMap<std::String, Json>&;

    public:
        auto as_boolean() -> bool&;
        auto as_integer() -> int&;   
        auto as_decimal() -> double&;
        auto as_string() -> std::String&;
        auto as_array() -> std::Vector<Json>&;
        auto as_object() -> std::HashMap<std::String, Json>&;

    public:
        auto at(const std::String& key) const -> const Json&;
        auto at(const std::String& key) -> Json&;
        auto operator [] (const std::String& key) const  -> const Json&;
        auto operator [] (const std::String& key) -> const Json&;
        auto get(const std::String& key) const -> std::optional<const Json*>;
        auto get(const std::String& key) -> std::optional<Json*>;

        auto contains(const std::String& key) const -> bool;
        auto size() const -> std::usize;

        auto insert(std::String key, Json value) -> void;

    public:
        auto at(std::usize index) const -> const Json&;
        auto at(std::usize index) -> Json&;
        auto operator [] (std::usize index) const -> const Json&;
        auto operator [] (std::usize index) -> Json&;
        auto get(std::usize index) const -> std::optional<const Json*>;
        auto get(std::usize index) -> std::optional<Json*>;

        auto len() const -> std::usize;
        auto push(Json json) -> void;

    public:
        auto to_string() -> std::String;
        auto type() const noexcept -> JsonType;

    public:
        template <typename Value>
        auto get_to(Value& v) const -> void {
            kiwi::serde::Deserialize<Json, Value>::from(*this, v);
        }

        template <typename Value>
        auto set_to(const Value& v) -> void {
            kiwi::serde::Serialize<Json, Value>::to(*this, v);
        }

    private:
        auto do_to_string(std::usize indent = 0, bool hasName = false) const -> std::String;
        void ensure_tyep(JsonType type) const;
        auto check_type(JsonType type) const -> bool;
        auto type_index() const noexcept -> std::usize;

    private:
        struct Null {};
        std::variant<
            Null,
            bool,
            int,
            double,
            std::String,
            std::Vector<Json>,
            std::HashMap<std::String, Json>
        > _value;
    };

}

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, bool> {
    static void from(const kiwi::serde::Json& j, bool& value) {
        value = j.as_boolean();
    }
};

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, int> {
    static void from(const kiwi::serde::Json& j, int& value) {
        value = j.as_integer();
    }
};

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::i64> {
    static void from(const kiwi::serde::Json& j, std::i64& value) {
        value = static_cast<std::i64>(j.as_integer());
    }
};

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::usize> {
    static void from(const kiwi::serde::Json& j, std::usize& value) {
        value = static_cast<std::usize>(j.as_integer());
    }
};

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, double> {
    static void from(const kiwi::serde::Json& j, double& value) {
        value = j.as_decimal();
    }
};

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::String> {
    static void from(const Json& j, std::String& value) {
        value = j.as_string();
    }
};

template <>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::FilePath> {
    static void from(const kiwi::serde::Json& j, std::FilePath& value) {
        value = j.as_string();
    }
};

template <typename Value>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::Vector<Value>> {
    static void from(const kiwi::serde::Json& json, std::Vector<Value>& value) {
        auto arr = json.as_array();
        for (auto& j : arr) {
            auto v = Value{};
            kiwi::serde::Deserialize<kiwi::serde::Json, Value>::from(j, v);
            value.emplace_back(std::move(v));
        }
    }
};

template <typename Value>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::HashMap<std::String, Value>> {
    static void from(const Json& json, std::HashMap<std::String, Value>& value) {
        auto& map = json.as_object();
        for (auto& [key, j] : map) {
            auto pair = value.emplace(key, Value{});
            kiwi::serde::Deserialize<kiwi::serde::Json, Value>::from(j, pair.first->second);
        }
    }
};

template <typename Value>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::map<std::String, Value>> {
    static void from(const Json& json, std::map<std::String, Value>& value) {
        auto& map = json.as_object();
        for (auto& [key, j] : map) {
            auto pair = value.emplace(key, Value{});
            kiwi::serde::Deserialize<kiwi::serde::Json, Value>::from(j, pair.first->second);
        }
    }
};

template <typename Key, typename Value>
struct kiwi::serde::Deserialize<kiwi::serde::Json, std::map<Key, Value>> {
    static void from(const Json& json, std::map<Key, Value>& value) {
        auto& map = json.as_object();
        for (auto& [key, j] : map) {
            auto keyv = Key::fromString(key);
            auto pair = value.emplace(keyv, Value{});
            kiwi::serde::Deserialize<kiwi::serde::Json, Value>::from(j, pair.first->second);
        }
    }
};