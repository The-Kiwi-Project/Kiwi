#include "circuit/basedie.hh"
#include "debug/console.hh"
#include "debug/debug.hh"
#include "hardware/interposer.hh"
#include <parse/reader/config/config.hh>
#include <parse/reader/module.hh>

using namespace kiwi;

static auto print_config(hardware::Interposer* i, circuit::BaseDie* b) -> void {
    console::println("Topdie:");
    console::println_fmt("\tsize: {}", b->topdies().size());
    auto index = 0;
    for (auto& [name, topdie] : b->topdies()) {
        console::println_fmt("\tTopdie {}: ", index++);
        console::println_fmt("\t\tname: {}", name);
    }
    console::println("");

    console::println("Topdie Inst:");
    console::println_fmt("\tsize: {}", b->topdie_insts().size());
    index = 0;
    for (auto& [name, topdie_inst] : b->topdie_insts()) {
        console::println_fmt("\tTopdie Inst {}: ", index++);
        console::println_fmt("\t\tname: {}", name);
        console::println_fmt("\t\tcoord: {}", topdie_inst->tob()->coord());
    }
    console::println("");

    console::println("Net:");
    console::println_fmt("\tsize: {}", b->nets().size());
    console::println("");
}

static auto test_case1() -> void try {
    auto [i, b] = parse::read_config("../test/config/case1");
    print_config(i.get(), b.get());
}
THROW_UP_WITH("test_case1")

static auto test_case2() -> void  try{
    auto [i, b] = parse::read_config("../test/config/case2");
    print_config(i.get(), b.get());
}
THROW_UP_WITH("test_case2")

void test_config_main() 
try {
    test_case1();
    test_case2();
}
THROW_UP_WITH("test_config_main")