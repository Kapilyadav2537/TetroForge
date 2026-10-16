/** @file test_vec2i.cpp  @brief Vec2i operators. @note SRS: §7.2, §7.6 (operator overloading). */
#include <catch2/catch_test_macros.hpp>

#include <sstream>

#include <tf/core/Vec2i.hpp>

using tf::core::Vec2i;

TEST_CASE("Vec2i defaults to the origin", "[vec2i]") {
    const Vec2i v{};
    const Vec2i zero{0, 0};
    CHECK(v == zero);
}

TEST_CASE("Vec2i arithmetic is component-wise", "[vec2i]") {
    const Vec2i a{3, 4};
    const Vec2i b{-1, 2};
    const Vec2i sum{2, 6};
    const Vec2i diff{4, 2};
    const Vec2i negated{-3, -4};
    CHECK(a + b == sum);
    CHECK(a - b == diff);
    CHECK(-a == negated);
}

TEST_CASE("Vec2i comparison", "[vec2i]") {
    const Vec2i a{1, 2};
    const Vec2i same{1, 2};
    const Vec2i other{2, 1};
    CHECK(a == same);
    CHECK(a != other);
}

TEST_CASE("Vec2i streams as (x, y)", "[vec2i]") {
    std::ostringstream out;
    out << Vec2i{3, -4};
    CHECK(out.str() == "(3, -4)");
}
