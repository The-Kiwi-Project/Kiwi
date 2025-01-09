#include <std/integer.hh>
#include "./utilty.hh"
#include "hardware/tob/tobmux.hh"
#include "hardware/tob/tob.hh"
#include <hardware/bump/bump.hh>

using namespace kiwi::hardware;

static void test_tob_mux_available_output_indexes() {
    auto tobmux = TOBMux{8};
    auto res = tobmux.available_output_indexes();
    ASSERT_EQ(res.size(), 8);

    std::usize v1[] = {0, 1, 2, 3, 4, 5, 6, 7};
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(res[i], v1[i]);
    }

    tobmux.registerr(0)->set(7);
    std::usize v2[] = {0, 1, 2, 3, 4, 5, 6};
    res = tobmux.available_output_indexes();
    ASSERT_EQ(res.size(), 7);
    for (int i = 0; i < 7; ++i) {
        ASSERT_EQ(res[i], v2[i]);
    }

    tobmux.registerr(0)->set(3);
    std::usize v3[] = {0, 1, 2, 4, 5, 6, 7};
    res = tobmux.available_output_indexes();
    ASSERT_EQ(res.size(), 7);
    for (int i = 0; i < 7; ++i) {
        ASSERT_EQ(res[i], v3[i]);
    }
}

static void test_available_output_indexes_1() {
    auto tob = TOB{0, 0};

    for (std::usize b_index = 0; b_index < 128; ++b_index) {
        auto cs = tob.available_connectors_bump_to_track(b_index);
        std::cout << b_index << " : " << cs.size() << std::endl;
        cs[0].connect();
    }
}

static void test_available_output_indexes_2() {
    auto tob = TOB{0, 0};
    auto cs = tob.available_connectors(0, TOBSignalDirection::BumpToTrack);
    ASSERT_EQ(cs.size(), 128);
    cs[0].connect();

    cs = tob.available_connectors(0, TOBSignalDirection::BumpToTrack);
    ASSERT_EQ(cs.size(), 0);
}

static void test_b_index_map_t_index() {
    auto tob = TOB{0, 0};

    ASSERT(!tob.bump_index_map_track_index(0));
    
    auto cs = tob.available_connectors_track_to_bump(0);
    cs[17].connect();

    ASSERT(tob.bump_index_map_track_index(0).has_value());
    ASSERT_EQ(cs[17].track_index(), *tob.bump_index_map_track_index(0));
}

static void test_randomly_map_remain_indexes() {
    auto tobmux = TOBMux{8};
    for (std::usize i = 0; i < 8; ++i) {
        ASSERT(!tobmux.index_map(i).has_value());
    }

    tobmux.registerr(0)->set(6);
    ASSERT(tobmux.index_map(0).has_value());

    tobmux.registerr(1)->set(3);
    ASSERT(tobmux.index_map(1).has_value());

    tobmux.randomly_map_remain_indexes();
    for (std::usize i = 0; i < 8; ++i) {
        ASSERT(tobmux.index_map(i).has_value());
    }
}

void test_tob_main() {
    test_tob_mux_available_output_indexes();
    test_available_output_indexes_1();
    test_available_output_indexes_2();
    test_b_index_map_t_index();
    test_randomly_map_remain_indexes();
}