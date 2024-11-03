#pragma once 

#include <optional>
#include <tuple>
#include <utility>
#include <variant>
#include <functional>

namespace std {

    template <typename T>
    using Option = optional<T>;

    template <typename T1, typename T2>
    using Pair = pair<T1, T2>;
    
    template <typename... Ts>
    using Tuple = tuple<Ts...>;

    template <typename... Ts>
    using Variant = variant<Ts...>;

    template <typename... Ts>
    using Function = function<Ts...>;

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template <typename Val, typename... Ts>
    auto match(Val &&val, Ts... ts) {
        return std::visit(overloaded{ts...}, std::forward<Val>(val));
    }

}