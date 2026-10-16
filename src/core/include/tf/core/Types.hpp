#pragma once
/**
 * @file Types.hpp
 * @brief Common enums, constants, and geometry utilities shared across Core.
 *
 * Defines fundamental gameplay primitives:
 * - Playfield dimensions (10x20 visible + 20 hidden spawn rows)
 * - The seven standard Tetromino pieces (I, O, T, S, Z, J, L)
 * - Super Rotation System (SRS) 4-state rotation orientations
 * - Board cell representations and conversion functions
 */
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace tf::core {

// --- Playfield Dimensions ---
/// Standard width of the Tetris playfield (10 columns).
inline constexpr int kBoardWidth = 10;

/// Number of visible rows displayed on screen (20 rows).
inline constexpr int kVisibleHeight = 20;

/// Hidden buffer rows above visible playfield where tetrominoes spawn (20 rows).
inline constexpr int kHiddenHeight = 20;

/// Total simulated rows (20 visible + 20 hidden = 40 total rows).
inline constexpr int kTotalHeight = kVisibleHeight + kHiddenHeight;

// --- Piece Definitions ---
/**
 * @brief Identifiers for the seven official tetromino shapes.
 * Uses strongly-typed uint8_t enum class for memory compactness and type safety.
 */
enum class PieceType : std::uint8_t { I, O, T, S, Z, J, L };

/// Total count of unique piece types.
inline constexpr std::size_t kPieceTypeCount = 7;

/// Fixed-order array of all 7 piece types, used for loops and 7-bag generation.
inline constexpr std::array<PieceType, kPieceTypeCount> kAllPieceTypes{
    {PieceType::I, PieceType::O, PieceType::T, PieceType::S, PieceType::Z, PieceType::J,
     PieceType::L}};

// --- Rotation States ---
/**
 * @brief The four rotation orientations defined by the Super Rotation System (SRS).
 * - Spawn: Initial spawn orientation (0 degrees)
 * - Right: One quarter-turn clockwise (90 degrees)
 * - Flip:  180-degree turn
 * - Left:  One quarter-turn counter-clockwise (270 degrees)
 */
enum class Rotation : std::uint8_t { Spawn, Right, Flip, Left };

/** @brief Rotation direction requested by the player. */
enum class Spin : std::uint8_t { Clockwise, CounterClockwise };

/**
 * @brief Contents of a single board grid cell.
 * Empty is 0 so default zero-initialized memory represents an empty board.
 * Filled cells store which piece type locked into them to support color themes.
 */
enum class Cell : std::uint8_t { Empty, I, O, T, S, Z, J, L };

/**
 * @brief Advances rotation one quarter-turn clockwise: (r + 1) mod 4.
 */
[[nodiscard]] constexpr Rotation rotatedCw(Rotation r) noexcept {
    return static_cast<Rotation>((static_cast<unsigned>(r) + 1U) % 4U);
}

/**
 * @brief Advances rotation one quarter-turn counter-clockwise: (r + 3) mod 4.
 * Adding 3 modulo 4 is mathematically equivalent to subtracting 1 without underflow.
 */
[[nodiscard]] constexpr Rotation rotatedCcw(Rotation r) noexcept {
    return static_cast<Rotation>((static_cast<unsigned>(r) + 3U) % 4U);
}

/**
 * @brief Converts a Rotation enum into a zero-based array index (0..3).
 */
[[nodiscard]] constexpr std::size_t toIndex(Rotation r) noexcept {
    return static_cast<std::size_t>(r);
}

/**
 * @brief Converts a piece type to its corresponding non-empty grid Cell value.
 */
[[nodiscard]] constexpr Cell toCell(PieceType type) noexcept {
    return static_cast<Cell>(static_cast<unsigned>(type) + 1U);
}

/**
 * @brief Returns the single-letter character representing a piece type ('I', 'O', etc.).
 */
[[nodiscard]] constexpr char toChar(PieceType type) noexcept {
    constexpr std::string_view kLetters{"IOTSZJL"};
    return kLetters[static_cast<std::size_t>(type)];
}

}  // namespace tf::core
