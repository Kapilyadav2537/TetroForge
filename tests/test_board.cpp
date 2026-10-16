/**
 * @file test_board.cpp
 * @brief Board: placement, locking, rotation with kicks, line clearing.
 * @note SRS: FR-1.1, FR-1.4, FR-1.9; test cases TC-01 and TC-02 of SRS section 9.2.
 */
#include <catch2/catch_test_macros.hpp>

#include <stdexcept>

#include <tf/core/Board.hpp>

#include "TestHelpers.hpp"

using namespace tf::core;
using namespace tf::test;

TEST_CASE("board dimensions follow FR-1.1", "[board]") {
    STATIC_REQUIRE(kBoardWidth == 10);
    STATIC_REQUIRE(kVisibleHeight == 20);
    STATIC_REQUIRE(kHiddenHeight == 20);
    STATIC_REQUIRE(kTotalHeight == 40);
}

TEST_CASE("a new board is empty", "[board]") {
    const Board board;
    for (int y = 0; y < kTotalHeight; ++y) {
        CHECK_FALSE(board.isRowFull(y));
        for (int x = 0; x < kBoardWidth; ++x) {
            CHECK(board.cellAt(x, y) == Cell::Empty);
        }
    }
}

TEST_CASE("cellAt outside the board throws", "[board]") {
    const Board board;
    REQUIRE_THROWS_AS(board.cellAt(-1, 0), std::out_of_range);
    REQUIRE_THROWS_AS(board.cellAt(kBoardWidth, 0), std::out_of_range);
    REQUIRE_THROWS_AS(board.cellAt(0, kTotalHeight), std::out_of_range);
}

TEST_CASE("canPlace enforces the walls floor and top", "[board]") {
    const Board board;
    // O piece cells sit at box x 1..2 and y 1..2.
    CHECK_FALSE(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{-2, 10})));
    CHECK(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{-1, 10})));
    CHECK(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{7, 10})));
    CHECK_FALSE(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{8, 10})));
    CHECK_FALSE(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{3, -2})));
    CHECK(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{3, -1})));
    CHECK(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{3, 37})));
    CHECK_FALSE(board.canPlace(*makePiece(PieceType::O, Rotation::Spawn, Vec2i{3, 38})));
}

TEST_CASE("lock writes the piece's cell type and blocks that space", "[board]") {
    Board board;
    lockPiece(board, PieceType::T, Rotation::Spawn, Vec2i{3, 0});
    // T spawn cells: (0,1) (1,1) (2,1) (1,2) -> board (3,1) (4,1) (5,1) (4,2)
    CHECK(board.cellAt(3, 1) == Cell::T);
    CHECK(board.cellAt(4, 2) == Cell::T);
    CHECK(board.cellAt(3, 0) == Cell::Empty);
    CHECK_FALSE(board.canPlace(*makePiece(PieceType::T, Rotation::Spawn, Vec2i{3, 0})));
}

TEST_CASE("lock rejects a piece that does not fit", "[board]") {
    Board board;
    lockPiece(board, PieceType::O, Rotation::Spawn, Vec2i{3, 0});
    const auto overlapping = makePiece(PieceType::O, Rotation::Spawn, Vec2i{3, 0});
    REQUIRE_THROWS_AS(board.lock(*overlapping), std::logic_error);
}

TEST_CASE("clearLines on a board without full rows changes nothing", "[board]") {
    Board board;
    lockPiece(board, PieceType::O, Rotation::Spawn, Vec2i{3, 0});
    CHECK(board.clearLines() == 0);
    CHECK(board.cellAt(4, 1) == Cell::O);
}

TEST_CASE("TC-02 four full rows clear and the rows above fall", "[board][clear]") {
    Board board;
    for (int column = 0; column < 9; ++column) {
        lockVerticalI(board, column, 0);  // fills columns 0..8 of rows 0..3
    }
    // An O piece resting above the stack: occupies columns 0-1, rows 4-5.
    lockPiece(board, PieceType::O, Rotation::Spawn, Vec2i{-1, 3});
    REQUIRE(board.clearLines() == 0);  // column 9 is still open

    lockVerticalI(board, 9, 0);  // the "drop an I piece" step
    CHECK(board.isRowFull(0));
    CHECK(board.isRowFull(3));
    REQUIRE(board.clearLines() == 4);

    // The O fell by exactly four rows: it now sits in rows 0-1.
    CHECK(board.cellAt(0, 0) == Cell::O);
    CHECK(board.cellAt(1, 0) == Cell::O);
    CHECK(board.cellAt(0, 1) == Cell::O);
    CHECK(board.cellAt(1, 1) == Cell::O);
    CHECK(board.cellAt(2, 0) == Cell::Empty);
    CHECK(board.cellAt(0, 2) == Cell::Empty);
    CHECK(board.cellAt(0, 4) == Cell::Empty);
    CHECK(board.cellAt(0, 5) == Cell::Empty);
}

TEST_CASE("clear empties the board", "[board]") {
    Board board;
    lockPiece(board, PieceType::O, Rotation::Spawn, Vec2i{3, 0});
    board.clear();
    CHECK(board.cellAt(4, 1) == Cell::Empty);
}

TEST_CASE("rotation in open space needs no kick", "[board][kicks]") {
    const Board board;
    auto piece = makePiece(PieceType::T, Rotation::Spawn, Vec2i{3, 10});
    REQUIRE(board.tryRotate(*piece, Spin::Clockwise));
    const Vec2i expected{3, 10};
    CHECK(piece->rotation() == Rotation::Right);
    CHECK(piece->position() == expected);
}

TEST_CASE("TC-01 vertical I against the left wall kicks inward", "[board][kicks]") {
    const Board board;
    auto piece = makePiece(PieceType::I, Rotation::Right, Vec2i{-2, 10});  // occupies column 0
    REQUIRE(board.canPlace(*piece));

    REQUIRE(board.tryRotate(*piece, Spin::CounterClockwise));
    const Vec2i expected{0, 10};  // kick (+2, 0) from the R -> 0 row
    CHECK(piece->rotation() == Rotation::Spawn);
    CHECK(piece->position() == expected);
    CHECK(board.canPlace(*piece));
    for (const Vec2i cell : piece->worldCells()) {
        CHECK(cell.x >= 0);
        CHECK(cell.x < kBoardWidth);
    }
}

TEST_CASE("TC-01 vertical I against the right wall kicks inward", "[board][kicks]") {
    const Board board;
    auto piece = makePiece(PieceType::I, Rotation::Left, Vec2i{8, 10});  // occupies column 9
    REQUIRE(board.canPlace(*piece));

    REQUIRE(board.tryRotate(*piece, Spin::Clockwise));
    const Vec2i expected{6, 10};  // third test of the L -> 0 row: (-2, 0)
    CHECK(piece->rotation() == Rotation::Spawn);
    CHECK(piece->position() == expected);
    CHECK(board.canPlace(*piece));
}

TEST_CASE("flat I on the floor kicks upward when turned", "[board][kicks]") {
    const Board board;
    auto piece = makePiece(PieceType::I, Rotation::Flip, Vec2i{3, -1});  // cells on row 0
    REQUIRE(board.canPlace(*piece));

    REQUIRE(board.tryRotate(*piece, Spin::Clockwise));
    const Vec2i expected{5, 0};  // fourth test of the 2 -> L row: (+2, +1)
    CHECK(piece->rotation() == Rotation::Left);
    CHECK(piece->position() == expected);
    CHECK(board.canPlace(*piece));
}

TEST_CASE("a blocked rotation fails and leaves the piece unchanged", "[board][kicks]") {
    Board board;
    for (int column = 0; column < kBoardWidth; ++column) {
        for (int bottom = 5; bottom <= 13; bottom += 4) {
            lockVerticalI(board, column, bottom);  // solid block of rows 5..16
        }
    }
    // Every kick candidate of this T lies inside the solid block.
    auto piece = makePiece(PieceType::T, Rotation::Spawn, Vec2i{3, 8});
    const Vec2i before{3, 8};
    CHECK_FALSE(board.tryRotate(*piece, Spin::Clockwise));
    CHECK(piece->rotation() == Rotation::Spawn);
    CHECK(piece->position() == before);
}
