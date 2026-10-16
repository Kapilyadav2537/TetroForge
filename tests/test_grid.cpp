/** @file test_grid.cpp  @brief Grid<T> container. @note SRS: §7.2 (Grid), §7.6. */
#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include <tf/core/Grid.hpp>

using IntGrid = tf::core::Grid<int, 4, 3>;

TEST_CASE("Grid reports its fixed size", "[grid]") {
    STATIC_REQUIRE(IntGrid::width() == 4);
    STATIC_REQUIRE(IntGrid::height() == 3);
}

TEST_CASE("Grid starts value-initialised", "[grid]") {
    const IntGrid grid;
    for (int y = 0; y < IntGrid::height(); ++y) {
        for (int x = 0; x < IntGrid::width(); ++x) {
            CHECK(grid(x, y) == 0);
        }
    }
}

TEST_CASE("Grid stores each cell independently", "[grid]") {
    IntGrid grid;
    grid(1, 2) = 7;
    grid(3, 0) = 9;
    CHECK(grid(1, 2) == 7);
    CHECK(grid(3, 0) == 9);
    CHECK(grid(2, 1) == 0);
}

TEST_CASE("Grid::fill overwrites everything", "[grid]") {
    IntGrid grid;
    grid(0, 0) = 5;
    grid.fill(2);
    CHECK(grid(0, 0) == 2);
    CHECK(grid(3, 2) == 2);
}

TEST_CASE("Grid::inBounds matches the dimensions", "[grid]") {
    CHECK(IntGrid::inBounds(0, 0));
    CHECK(IntGrid::inBounds(3, 2));
    CHECK_FALSE(IntGrid::inBounds(4, 0));
    CHECK_FALSE(IntGrid::inBounds(0, 3));
    CHECK_FALSE(IntGrid::inBounds(-1, 0));
    CHECK_FALSE(IntGrid::inBounds(0, -1));
}

TEST_CASE("Grid access outside the bounds throws", "[grid]") {
    IntGrid grid;
    const IntGrid& constGrid = grid;
    REQUIRE_THROWS_AS(grid(4, 0), std::out_of_range);
    REQUIRE_THROWS_AS(grid(0, 3), std::out_of_range);
    REQUIRE_THROWS_AS(constGrid(-1, 0), std::out_of_range);
}
