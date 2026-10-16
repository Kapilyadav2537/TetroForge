#pragma once
/**
 * @file Vec2i.hpp
 * @brief 2D integer coordinate with overloaded operators.
 * @note SRS: §7.2 (Vec2i), §7.6 (operator overloading), NFR-3 (integers only).
 *
 * Coordinate convention used everywhere in Core: x grows to the right and
 * y grows UPWARD (row 0 is the floor). This matches the published SRS kick tables.
 */
#include <iosfwd>

namespace tf::core {

/** @brief A pair of ints. Aggregate, so `Vec2i{3, 4}` works. */
struct Vec2i {
    int x{0};
    int y{0};
};

/** @brief Component-wise sum. */
[[nodiscard]] constexpr Vec2i operator+(Vec2i a, Vec2i b) noexcept {
    return Vec2i{a.x + b.x, a.y + b.y};
}

/** @brief Component-wise difference. */
[[nodiscard]] constexpr Vec2i operator-(Vec2i a, Vec2i b) noexcept {
    return Vec2i{a.x - b.x, a.y - b.y};
}

/** @brief Negation (used to compare forward and reverse kick offsets). */
[[nodiscard]] constexpr Vec2i operator-(Vec2i v) noexcept {
    return Vec2i{-v.x, -v.y};
}

/** @brief Equality. */
[[nodiscard]] constexpr bool operator==(Vec2i a, Vec2i b) noexcept {
    return a.x == b.x && a.y == b.y;
}

/** @brief Inequality. */
[[nodiscard]] constexpr bool operator!=(Vec2i a, Vec2i b) noexcept {
    return !(a == b);
}

/** @brief Prints "(x, y)". Handy for test failure messages. */
std::ostream& operator<<(std::ostream& os, Vec2i v);

}  // namespace tf::core
