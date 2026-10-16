#pragma once
/**
 * @file TestHelpers.hpp
 * @brief Small helpers shared by Core tests.
 * Note: avoid && and || inside CHECK/REQUIRE (Catch2 cannot decompose them).
 */
#include <algorithm>
#include <memory>

#include <tf/core/Board.hpp>
#include <tf/core/PieceFactory.hpp>

namespace tf::test {

/// Builds a piece with an explicit rotation and origin.
inline std::unique_ptr<core::Tetromino> makePiece(core::PieceType type, core::Rotation rotation,
                                                  core::Vec2i origin) {
    auto piece = core::PieceFactory::create(type);
    piece->setRotation(rotation);
    piece->setPosition(origin);
    return piece;
}

/// Builds a piece and locks it into the board.
inline void lockPiece(core::Board& board, core::PieceType type, core::Rotation rotation,
                      core::Vec2i origin) {
    board.lock(*makePiece(type, rotation, origin));
}

/// Locks a vertical I piece occupying `column` and rows bottomRow..bottomRow+3.
inline void lockVerticalI(core::Board& board, int column, int bottomRow) {
    // In the Right orientation the I's cells sit at box x = 2.
    lockPiece(board, core::PieceType::I, core::Rotation::Right, core::Vec2i{column - 2, bottomRow});
}

/// Sorts cells by (y, x) so shapes can be compared as sets.
inline core::CellArray sorted(core::CellArray cells) {
    std::sort(cells.begin(), cells.end(), [](core::Vec2i a, core::Vec2i b) {
        return a.y != b.y ? a.y < b.y : a.x < b.x;
    });
    return cells;
}

}  // namespace tf::test
