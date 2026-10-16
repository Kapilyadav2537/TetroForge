#pragma once
/**
 * @file Types.hpp
 * @brief Enums and board-size constants shared by all of Core.
 * @note SRS: FR-1.1 (10x20 + 20 hidden rows), FR-1.2 (seven pieces),
 *       FR-1.4 (rotation states), §7.6 (enum class), NFR-3 (no floating point).
 */
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace tf::core {

/// Playfield width in cells (FR-1.1).
inline constexpr int kBoardWidth = 10;
/// Rows the player can see (FR-1.1).
inline constexpr int kVisibleHeight = 20;
/// Hidden rows above the visible field where pieces spawn (FR-1.1).
inline constexpr int kHiddenHeight = 20;
/// Total simulated rows.
inline constexpr int kTotalHeight = kVisibleHeight + kHiddenHeight;

/** @brief The seven tetrominoes (FR-1.2). */
enum class PieceType : std::uint8_t { I, O, T, S, Z, J, L };

/// Number of distinct piece types.
inline constexpr std::size_t kPieceTypeCount = 7;

/// All piece types in a fixed order, for loops.
inline constexpr std::array<PieceType, kPieceTypeCount> kAllPieceTypes{
    {PieceType::I, PieceType::O, PieceType::T, PieceType::S, PieceType::Z, PieceType::J,
     PieceType::L}};

/**
 * @brief The four SRS orientations (FR-1.4).
 * Spawn = 0, Right = R (one CW turn), Flip = 2, Left = L (one CCW turn).
 */
enum class Rotation : std::uint8_t { Spawn, Right, Flip, Left };

/** @brief Direction of a rotation request (FR-1.4). */
enum class Spin : std::uint8_t { Clockwise, CounterClockwise };

/**
 * @brief Content of one board cell. Empty is 0 so a zeroed grid is an empty board.
 * Filled cells remember which piece created them (themes need that, FR-4.6).
 */
enum class Cell : std::uint8_t { Empty, I, O, T, S, Z, J, L };

/** @brief Next orientation clockwise. */
[[nodiscard]] constexpr Rotation rotatedCw(Rotation r) noexcept {
    return static_cast<Rotation>((static_cast<unsigned>(r) + 1U) % 4U);
}

/** @brief Next orientation counter-clockwise. */
[[nodiscard]] constexpr Rotation rotatedCcw(Rotation r) noexcept {
    return static_cast<Rotation>((static_cast<unsigned>(r) + 3U) % 4U);
}

/** @brief Rotation as an array index 0..3. */
[[nodiscard]] constexpr std::size_t toIndex(Rotation r) noexcept {
    return static_cast<std::size_t>(r);
}

/** @brief The filled-cell value that belongs to a piece type. */
[[nodiscard]] constexpr Cell toCell(PieceType type) noexcept {
    return static_cast<Cell>(static_cast<unsigned>(type) + 1U);
}

/** @brief Letter used for a piece in debug output ("IOTSZJL"). */
[[nodiscard]] constexpr char toChar(PieceType type) noexcept {
    constexpr std::string_view kLetters{"IOTSZJL"};
    return kLetters[static_cast<std::size_t>(type)];
}

}  // namespace tf::core
