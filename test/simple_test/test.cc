#include <exception>
#include <std/string.hh>
#include <std/collection.hh>
#include <debug/console.hh>
#include <cassert>
#include <sys/types.h>

using TestFunction = void(*)(void);

extern void test_cob_main();
extern void test_interposer_main();
extern void test_tob_main();
extern void test_router_main();
extern void test_placer_main();
extern void test_debug_main();
extern void test_config_main();
extern void test_kiwi_main();

#define REGISTER_TEST(test_name)\
functions.emplace(#test_name, & test_##test_name##_main);\
if (target == #test_name) {\
    kiwi::console::println_fmt("Run test '{}'", #test_name);\
    test_##test_name##_main();\
    return 0;\
}\

int main(int argc, char** argv) 
try {
    assert(argc == 2);

    auto functions = std::HashMap<std::StringView, TestFunction>{};
    auto target = std::StringView{argv[1]};

    REGISTER_TEST(cob)
    REGISTER_TEST(tob)
    REGISTER_TEST(interposer)
    REGISTER_TEST(router)
    REGISTER_TEST(placer)
    REGISTER_TEST(debug)
    REGISTER_TEST(config)
    REGISTER_TEST(kiwi)

    if (target == "all") {
        for (auto [test_name, test_func] : functions) {
            kiwi::console::println_fmt("Run test '{}'", test_name);
            test_func();
            kiwi::console::println("");
        }
        return 0;
    }

    kiwi::console::println_fmt("No exit test target '{}'", target);
    return 0;
}
catch (const std::exception& err) {
    kiwi::console::error_fmt("Error in test: {}", err.what());
}