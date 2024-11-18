#pragma once

#include <array>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <map>
#include <set>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <bitset>

namespace std {

    template <typename T, std::size_t N>
    using Array = array<T, N>;

    template <typename T>
    using Vector = vector<T>;

    template <typename T>
    using List = list<T>;

    template <typename T>
    using Queue = queue<T>;

    template <typename T>
    using Stack = stack<T>;

    template <typename K, typename V>
    using Map = map<K, V>;

    template <typename T>
    using Set = set<T>;

    template <typename K, typename V>
    using HashMap = unordered_map<K, V>;

    template <typename T>
    using HashSet = unordered_set<T>;

    template <typename T>
    using Span = span<T>;

    template <std::size_t N>
    using Bits = bitset<N>;
    
}