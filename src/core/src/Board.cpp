/** @file Board.cpp  @note SRS: FR-1.1, FR-1.4, FR-1.9, §7.2. */
#include <tf/core/Board.hpp>

#include <algorithm>
#include <stdexcept>

namespace tf::core {

bool Board::isFree(Vec2i cell) const {
    return GridType::inBounds(cell.x, cell.y) && grid_(cell.x, cell.y) == Cell::Empty;
}

bool Board::canPlace(const Tetromino& piece) const {
    return canPlace(piece, piece.position(), piece.rotation());
}

bool Board::canPlace(const Tetromino& piece, Vec2i origin, Rotation rotation) const {
    const CellArray cells = piece.worldCells(origin, rotation);
    return std::all_of(cells.begin(), cells.end(), [this](Vec2i c) { return isFree(c); });
}

bool Board::tryRotate(Tetromino& piece, Spin spin) const {
    const Rotation from = piece.rotation();
    const Rotation to = (spin == Spin::Clockwise) ? rotatedCw(from) : rotatedCcw(from);
    for (const Vec2i offset : piece.kicks(from, to)) {
        const Vec2i origin = piece.position() + offset;
        if (canPlace(piece, origin, to)) {
            piece.setPosition(origin);
            piece.setRotation(to);
            return true;
        }
    }
    return false;
}

void Board::lock(const Tetromino& piece) {
    if (!canPlace(piece)) {
        throw std::logic_error("Board::lock: piece does not fit");
    }
    const Cell value = toCell(piece.type());
    for (const Vec2i c : piece.worldCells()) {
        grid_(c.x, c.y) = value;
    }
}

int Board::clearLines() {
    int write = 0;  // next row that receives a surviving row
    for (int read = 0; read < kTotalHeight; ++read) {
        if (isRowFull(read)) {
            continue;  // dropped: not copied down
        }
        if (write != read) {
            copyRow(read, write);
        }
        ++write;
    }
    const int cleared = kTotalHeight - write;
    for (int y = write; y < kTotalHeight; ++y) {
        clearRow(y);  // rows above the compacted stack are now empty
    }
    return cleared;
}

Cell Board::cellAt(int x, int y) const {
    return grid_(x, y);
}

bool Board::isRowFull(int y) const {
    for (int x = 0; x < kBoardWidth; ++x) {
        if (grid_(x, y) == Cell::Empty) {
            return false;
        }
    }
    return true;
}

void Board::clear() {
    grid_.fill(Cell::Empty);
}

void Board::copyRow(int from, int to) {
    for (int x = 0; x < kBoardWidth; ++x) {
        grid_(x, to) = grid_(x, from);
    }
}

void Board::clearRow(int y) {
    for (int x = 0; x < kBoardWidth; ++x) {
        grid_(x, y) = Cell::Empty;
    }
}

}  // namespace tf::core
