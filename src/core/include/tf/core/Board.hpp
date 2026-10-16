#pragma once
/**
 * @file Board.hpp
 * @brief The playfield: collision checks, locking pieces, clearing lines.
 * @note SRS: FR-1.1, FR-1.4, FR-1.9, §7.2 (Board), §7.6 (encapsulation: the grid is private).
 */
#include <tf/core/Grid.hpp>
#include <tf/core/Tetromino.hpp>
#include <tf/core/Types.hpp>

namespace tf::core {

/**
 * @brief 10 columns x 40 rows (20 visible + 20 hidden). Row 0 is the floor.
 *
 * Board knows geometry only. It does not know about score, levels or time.
 */
class Board {
public:
    /** @brief True if all four cells of @p piece are in bounds and empty. */
    [[nodiscard]] bool canPlace(const Tetromino& piece) const;

    /** @brief Same test for a hypothetical origin and rotation (piece is not changed). */
    [[nodiscard]] bool canPlace(const Tetromino& piece, Vec2i origin, Rotation rotation) const;

    /**
     * @brief Rotates @p piece with SRS wall kicks (FR-1.4).
     * Tries each kick offset in order; the first that fits is applied.
     * @return true if rotated; false if all five tests collide (piece unchanged).
     */
    [[nodiscard]] bool tryRotate(Tetromino& piece, Spin spin) const;

    /**
     * @brief Writes the piece's cells into the grid.
     * @throws std::logic_error if the piece does not fit (caller bug).
     * Does not clear lines; call clearLines() afterwards.
     */
    void lock(const Tetromino& piece);

    /**
     * @brief Removes every full row; rows above fall down (FR-1.9).
     * @return Number of rows removed (0 to 4 in normal play).
     */
    [[nodiscard]] int clearLines();

    /** @brief Cell content at (x, y). @throws std::out_of_range if outside the board. */
    [[nodiscard]] Cell cellAt(int x, int y) const;

    /** @brief True if every cell of row @p y is filled. @throws std::out_of_range. */
    [[nodiscard]] bool isRowFull(int y) const;

    /** @brief Empties the whole board. */
    void clear();

private:
    using GridType = Grid<Cell, kBoardWidth, kTotalHeight>;

    [[nodiscard]] bool isFree(Vec2i cell) const;
    void copyRow(int from, int to);
    void clearRow(int y);

    GridType grid_;
};

}  // namespace tf::core
