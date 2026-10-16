/**
 * @file Tetromino.cpp
 * @brief Implements coordinate translation from local piece space to board space.
 */
#include <tf/core/Tetromino.hpp>

namespace tf::core {

Tetromino::Tetromino(PieceType pieceType, Vec2i spawn) noexcept
    : type_(pieceType), position_(spawn) {}

CellArray Tetromino::worldCells() const {
    return worldCells(position_, rotation_);
}

CellArray Tetromino::worldCells(Vec2i origin, Rotation rotation) const {
    // Step 1: Retrieve the 4 in-box cell offsets for this rotation from the concrete subclass.
    CellArray result = cells(rotation);

    // Step 2: Translate each local offset by adding the world origin coordinates.
    for (Vec2i& cell : result) {
        cell = cell + origin;
    }
    return result;
}

}  // namespace tf::core
