#pragma once

template <int A, int B, int... Rest>
struct AllUnique {
    static constexpr bool value = (A != B) && AllUnique<A, Rest...>::value && AllUnique<B, Rest...>::value;
};

template <int A, int B>
struct AllUnique<A, B> {
    static constexpr bool value = (A != B);
};

template <int... Values>
constexpr void checkAllUnique() {
    static_assert(AllUnique<Values...>::value, "Constants must be unique!");
}