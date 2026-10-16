/** @file Tetromino.cpp  @note SRS: FR-1.2, §7.2. */
#include <tf/core/Tetromino.hpp>

namespace tf::core {

Tetromino::Tetromino(PieceType pieceType, Vec2i spawn) noexcept
    : type_(pieceType), position_(spawn) {}

CellArray Tetromino::worldCells() const {
    return worldCells(position_, rotation_);
}

CellArray Tetromino::worldCells(Vec2i origin, Rotation rotation) const {
    CellArray result = cells(rotation);  // virtual call: shape comes from the subclass
    for (Vec2i& cell : result) {
        cell = cell + origin;
    }
    return result;
}

}  // namespace tf::core
