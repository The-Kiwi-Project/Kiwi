#include <debug/debug.hh>

using namespace kiwi;

void test_debug_main() {
    debug::initial_log("E:/Kiwi/kiwi/kiwi.log");

    debug::debug_fmt("Hello {}!", "kiwi");
    debug::info_fmt("Hello {}!", "kiwi");
    debug::warning_fmt("Hello {}!", "kiwi");
    debug::error_fmt("Hello {}!", "kiwi");
    // debug::fatal_fmt("Hello {}!", "kiwi");
}