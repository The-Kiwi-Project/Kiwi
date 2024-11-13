#include <debug/debug.hh>
#include <std/exception.hh>

namespace kiwi {
    int main(int argc, char** argv);
}

int main(int argc, char** argv) try {
    return kiwi::main(argc, argv);
} catch (const std::Exception& err) {
    kiwi::debug::error_fmt("Kiwi failed: {}", err.what());
    std::exit(1);
}