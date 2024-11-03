#include "./utilty.hh"
#include "hardware/cob/cobunit.hh"
#include "hardware/cob/cob.hh"

using namespace kiwi::hardware;

static auto test_cob_unit() {
    auto cobunit = COBUnit{};
    auto connectors = cobunit.adjacent_connectors_from_left(0);
    ASSERT_EQ(connectors.size(), 3);
    for (auto& connector : connectors) {
        switch (connector.to_dir) {
            case COBDirection::Left: ASSERT(false) break;
            case COBDirection::Right: ASSERT_EQ(connector.to_index, 0) break;
            case COBDirection::Up: ASSERT_EQ(connector.to_index, 6) break;
            case COBDirection::Down: ASSERT_EQ(connector.to_index, 1) break;
        }
    }
}

template<>
void assert_eq<>(std::Tuple<std::usize, std::usize>&& t1, std::Tuple<std::usize, std::usize>&& t2) {
    assert_eq(std::get<0>(t1), std::get<0>(t2));
    assert_eq(std::get<1>(t1), std::get<1>(t2));
}

static auto test_cob_index_to_cobunit_info() -> void {
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Down, 104), {13, 0});
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Left, 0), {0, 0});
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Up, 0), {0, 0});
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Right, 78), {9, 6});
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Right, 29), {3, 5});
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Right, 30), {3, 6});
    assert_eq(COB::cob_index_to_cobunit_info(COBDirection::Right, 31), {3, 7});    
}

static auto test_cobunit_info_to_cob_index() -> void {
    ASSERT_EQ(COB::cobunit_info_to_cob_index(COBDirection::Left, {0, 0}), 0);
    ASSERT_EQ(COB::cobunit_info_to_cob_index(COBDirection::Up, {0, 0}), 0);
    ASSERT_EQ(COB::cobunit_info_to_cob_index(COBDirection::Right, {9, 6}), 78);
}

static auto test_track_index_map() -> void {
    ASSERT_EQ(COB::track_index_map(COBDirection::Down, 69, COBDirection::Right), 117);
    ASSERT_EQ(COB::track_index_map(COBDirection::Left, 2, COBDirection::Up), 50);
    ASSERT_EQ(COB::track_index_map(COBDirection::Down, 117, COBDirection::Right), 69);
    ASSERT_EQ(COB::track_index_map(COBDirection::Left, 18, COBDirection::Up), 34);
    ASSERT_EQ(COB::track_index_map(COBDirection::Down, 101, COBDirection::Right), 85);
    ASSERT_EQ(COB::track_index_map(COBDirection::Left, 34, COBDirection::Up), 18);
    ASSERT_EQ(COB::track_index_map(COBDirection::Up, 65, COBDirection::Right), 73);
    ASSERT_EQ(COB::track_index_map(COBDirection::Left, 0, COBDirection::Down), 8);
    ASSERT_EQ(COB::track_index_map(COBDirection::Left, 0, COBDirection::Up), 48);
    ASSERT_EQ(COB::track_index_map(COBDirection::Right, 0, COBDirection::Down), 48);
}

static auto test_cob_index_map() -> void {
    ASSERT_EQ(COB::cob_index_map(COBDirection::Up, 72, COBDirection::Right), 73);
    ASSERT_EQ(COB::cob_index_map(COBDirection::Up, 90, COBDirection::Right), 91);
    ASSERT_EQ(COB::cob_index_map(COBDirection::Down, 72, COBDirection::Right), 78);
    ASSERT_EQ(COB::cob_index_map(COBDirection::Down, 104, COBDirection::Right), 110);
}

static auto test_adjacent_connectors() -> void {
    auto cob = COB{0, 0};

    auto cs = cob.adjacent_connectors(COBDirection::Left, 0);
    ASSERT_EQ(cs.size(), 3);

    cs = cob.adjacent_connectors(COBDirection::Left, 0);
    ASSERT_EQ(cs.size(), 3);
}

void test_cob_main() {
    test_cob_unit();
    test_cob_index_to_cobunit_info();
    test_cobunit_info_to_cob_index();
    test_track_index_map();
    test_cob_index_map();
    test_adjacent_connectors();
}