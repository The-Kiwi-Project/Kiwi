#include "./random.hh"

#include <std/integer.hh>
#include <cstdlib>
#include <ctime>

namespace kiwi {

    struct RandomSeed {
        RandomSeed() {
            srand(time(nullptr));
        }
    };  

    static auto _ = RandomSeed{};

    auto random() -> double {
        auto value = static_cast<double>(rand());
        return value / (static_cast<double>(RAND_MAX));
    }

    auto random_i64(std::i64 min, std::i64 max) -> std::i64 {
        auto value = static_cast<std::i64>(rand());
        return min + (value % (max - min));
    }

}