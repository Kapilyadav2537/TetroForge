#pragma once
/**
 * @file Board.hpp
 * @brief Playfield representation: grid geometry, collision detection, piece locking, and line clearing.
 */
#include <tf/core/Grid.hpp>
#include <tf/core/Tetromino.hpp>
#include <tf/core/Types.hpp>

namespace tf::core {

/**
 * @brief 10-column by 40-row playfield matrix (20 visible lower rows, 20 upper buffer rows).
 * Row index 0 represents the floor; row 39 is the top buffer ceiling.
 *
 * Encapsulation: The underlying 2D grid storage is strictly private. External classes
 * query state through collision checks and line clearing operations.
 */
class Board {
public:
    /**
     * @brief Checks if a piece can be placed at its current position and orientation.
     * @return true if all four cells are in-bounds and on unoccupied cells.
     */
    [[nodiscard]] bool canPlace(const Tetromino& piece) const;

    /**
     * @brief Checks if a piece could be placed at an arbitrary hypothetical origin and rotation.
     * Does not mutate the piece.
     */
    [[nodiscard]] bool canPlace(const Tetromino& piece, Vec2i origin, Rotation rotation) const;

    /**
     * @brief Attempts to rotate a piece using Super Rotation System (SRS) wall kick offsets.
     * Evaluates up to 5 kick test offsets. The first offset without collisions is applied.
     * @param piece Tetromino to rotate and translate.
     * @param spin Direction of rotation (Clockwise or CounterClockwise).
     * @return true if rotation succeeded, false if all 5 kick tests failed (piece unchanged).
     */
    [[nodiscard]] bool tryRotate(Tetromino& piece, Spin spin) const;

    /**
     * @brief Commits a tetromino's cells permanently into the grid matrix.
     * @param piece Tetromino in a valid placement position.
     * @throws std::logic_error if the piece collides or is out of bounds.
     */
    void lock(const Tetromino& piece);

    /**
     * @brief Scans for full rows, removes them, and drops rows above downwards.
     * @return Total number of lines cleared (0 to 4 in standard play).
     */
    [[nodiscard]] int clearLines();

    /**
     * @brief Retrieves the cell content at matrix coordinate (x, y).
     * @throws std::out_of_range if coordinate is outside [0..9] x [0..39].
     */
    [[nodiscard]] Cell cellAt(int x, int y) const;

    /**
     * @brief Checks whether an entire horizontal row is filled with solid blocks.
     */
    [[nodiscard]] bool isRowFull(int y) const;

    /**
     * @brief Clears the entire board, resetting all cells to empty.
     */
    void clear();

private:
    using GridType = Grid<Cell, kBoardWidth, kTotalHeight>;

    [[nodiscard]] bool isFree(Vec2i cell) const;
    void copyRow(int from, int to);
    void clearRow(int y);

    GridType grid_;
};

}  // namespace tf::core
