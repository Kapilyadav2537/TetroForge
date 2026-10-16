#pragma once
/**
 * @file Grid.hpp
 * @brief Fixed-size 2D container with bounds-checked access.
 * @note SRS: §7.2 (Grid<T>), §7.6 (compile-time polymorphism, operator overloading).
 */
#include <array>
#include <cstddef>
#include <stdexcept>

namespace tf::core {

/**
 * @brief A Width x Height grid stored contiguously (row-major, row 0 first).
 * @tparam T      Element type; must be default-constructible.
 * @tparam Width  Number of columns (> 0).
 * @tparam Height Number of rows (> 0).
 *
 * Size is a template parameter, so the grid lives inside its owner with no heap use.
 */
template <typename T, int Width, int Height>
class Grid {
    static_assert(Width > 0 && Height > 0, "Grid dimensions must be positive");

public:
    /** @brief Number of columns. */
    [[nodiscard]] static constexpr int width() noexcept { return Width; }
    /** @brief Number of rows. */
    [[nodiscard]] static constexpr int height() noexcept { return Height; }

    /** @brief True when (x, y) lies inside the grid. */
    [[nodiscard]] static constexpr bool inBounds(int x, int y) noexcept {
        return x >= 0 && x < Width && y >= 0 && y < Height;
    }

    /**
     * @brief Mutable element access.
     * @throws std::out_of_range if (x, y) is outside the grid.
     */
    T& operator()(int x, int y) {
        checkBounds(x, y);
        return data_[index(x, y)];
    }

    /**
     * @brief Read-only element access.
     * @throws std::out_of_range if (x, y) is outside the grid.
     */
    [[nodiscard]] const T& operator()(int x, int y) const {
        checkBounds(x, y);
        return data_[index(x, y)];
    }

    /** @brief Sets every element to @p value. */
    void fill(const T& value) { data_.fill(value); }

private:
    static constexpr std::size_t kCount = static_cast<std::size_t>(Width) *
                                          static_cast<std::size_t>(Height);

    static constexpr std::size_t index(int x, int y) noexcept {
        return static_cast<std::size_t>(y * Width + x);
    }

    static void checkBounds(int x, int y) {
        if (!inBounds(x, y)) {
            throw std::out_of_range("Grid: index out of range");
        }
    }

    std::array<T, kCount> data_{};  // value-initialised: numbers 0, enums = first value
};

}  // namespace tf::core
