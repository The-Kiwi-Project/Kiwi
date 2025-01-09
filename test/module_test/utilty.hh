#pragma once
#include <iostream>
#include <cstdlib>
#include <cassert>

#define ASSERT(a)\
if (!(a)) {\
    std::cout << "assert failed in " << '\'' << __LINE__ << "' for: " << (a) << std::endl;\
    exit(-1);\
}

#define ASSERT_EQ(a, b)\
if ((a) != (b)){\
    std::cout << "assert_eq failed in " << '\'' << __LINE__ << "' for: " << (a) << " != " << (b) << std::endl;\
    exit(-2);\
}

#define ASSERT_NE(a, b)\
if ((a) == (b)) {\
    std::cout << "assert_ne failed in " << '\'' << __LINE__ << "' for: " << (a) << "== " << (b) << std::endl;\
    exit(-1);\
}

template<typename T> 
auto assert_eq(T&& t1, T&& t2) -> void {
    if (t1 != t2) {
        std::cout << "assert_eq failed for: " << t1 << " != " << t2 << std::endl;
        exit(-1);
    }
}