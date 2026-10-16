#include <tf/core/Board.hpp>

#include <algorithm>
#include <stdexcept>

namespace tf::core {

bool Board::isFree(Vec2i cell) const {
    // A cell is free if it lies within the board boundaries and does not contain a locked block.
    return GridType::inBounds(cell.x, cell.y) && grid_(cell.x, cell.y) == Cell::Empty;
}

bool Board::canPlace(const Tetromino& piece) const {
    return canPlace(piece, piece.position(), piece.rotation());
}

bool Board::canPlace(const Tetromino& piece, Vec2i origin, Rotation rotation) const {
    // Step 1: Compute absolute board coordinates of all 4 blocks at the given position & rotation.
    const CellArray cells = piece.worldCells(origin, rotation);

    // Step 2: Ensure every cell is within bounds and unoccupied.
    return std::all_of(cells.begin(), cells.end(), [this](Vec2i c) { return isFree(c); });
}

bool Board::tryRotate(Tetromino& piece, Spin spin) const {
    // Step 1: Calculate the source and target rotation states.
    const Rotation from = piece.rotation();
    const Rotation to = (spin == Spin::Clockwise) ? rotatedCw(from) : rotatedCcw(from);

    // Step 2: Retrieve the 5 SRS kick offset tests for this transition.
    const auto offsets = piece.kicks(from, to);

    // Step 3: Test each kick candidate offset in sequence (Test 1 through Test 5).
    for (const Vec2i offset : offsets) {
        const Vec2i testOrigin = piece.position() + offset;

        // Step 4: Check if the piece fits at this offset with the new rotation.
        if (canPlace(piece, testOrigin, to)) {
            // Apply translation and new rotation to the piece upon first success.
            piece.setPosition(testOrigin);
            piece.setRotation(to);
            return true;
        }
    }

    // Step 5: If all 5 kick tests collide, the rotation fails and the piece remains unchanged.
    return false;
}

void Board::lock(const Tetromino& piece) {
    // Step 1: Validate that the piece does not illegally overlap existing geometry.
    if (!canPlace(piece)) {
        throw std::logic_error("Board::lock: piece cannot be locked into an invalid position");
    }

    // Step 2: Map the piece type (I, J, L, O, S, T, Z) to its cell color representation.
    const Cell value = toCell(piece.type());

    // Step 3: Stamp all 4 tetromino cells permanently into the playfield grid.
    for (const Vec2i c : piece.worldCells()) {
        grid_(c.x, c.y) = value;
    }
}

int Board::clearLines() {
    // Uses a two-pointer in-place compaction algorithm (O(N) row traversals):
    // - 'read' scans every row from bottom (0) to top (39).
    // - 'write' points to the next available row slot to receive an un-cleared row.

    // Step 1: Compact surviving (non-full) rows downward.
    int write = 0;
    for (int read = 0; read < kTotalHeight; ++read) {
        if (isRowFull(read)) {
            // Full row: skip it (effectively deleting it from the stack).
            continue;
        }
        // If one or more rows below were cleared, copy row down.
        if (write != read) {
            copyRow(read, write);
        }
        ++write;
    }

    // Step 2: Calculate how many rows were cleared.
    const int cleared = kTotalHeight - write;

    // Step 3: Blank out all rows above the compacted stack up to the ceiling.
    for (int y = write; y < kTotalHeight; ++y) {
        clearRow(y);
    }

    // Step 4: Return total lines cleared.
    return cleared;
}

Cell Board::cellAt(int x, int y) const {
    return grid_(x, y);
}

bool Board::isRowFull(int y) const {
    // Check if any cell in row y is empty.
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
