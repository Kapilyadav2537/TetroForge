/**
 * @file test_pieces.cpp
 * @brief Tetromino hierarchy, factory and prototype copies.
 * @note SRS: FR-1.2, §7.2, §7.5 (Factory, Prototype).
 */
#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <memory>

#include <tf/core/Board.hpp>
#include <tf/core/PieceFactory.hpp>

#include "TestHelpers.hpp"

using namespace tf::core;
using namespace tf::test;

namespace {

int boxSizeOf(PieceType type) {
    return (type == PieceType::I || type == PieceType::O) ? 4 : 3;
}

Rotation rotationAt(std::size_t index) {
    return static_cast<Rotation>(index);
}

}  // namespace

TEST_CASE("factory creates the requested type at the Spawn rotation", "[pieces]") {
    for (const PieceType type : kAllPieceTypes) {
        INFO("piece " << toChar(type));
        const auto piece = PieceFactory::create(type);
        CHECK(piece->type() == type);
        CHECK(piece->rotation() == Rotation::Spawn);
    }
}

TEST_CASE("every piece has four distinct in-box cells in every rotation", "[pieces]") {
    for (const PieceType type : kAllPieceTypes) {
        INFO("piece " << toChar(type));
        const auto piece = PieceFactory::create(type);
        const int box = boxSizeOf(type);
        for (std::size_t r = 0; r < 4; ++r) {
            const CellArray cells = sorted(piece->cells(rotationAt(r)));
            for (std::size_t i = 0; i < cells.size(); ++i) {
                CHECK(cells[i].x >= 0);
                CHECK(cells[i].x < box);
                CHECK(cells[i].y >= 0);
                CHECK(cells[i].y < box);
                if (i > 0) {
                    CHECK(cells[i] != cells[i - 1]);
                }
            }
        }
    }
}

TEST_CASE("T piece shapes match the SRS diagrams", "[pieces]") {
    const auto piece = PieceFactory::create(PieceType::T);
    const CellArray spawn{{{0, 1}, {1, 1}, {2, 1}, {1, 2}}};
    const CellArray right{{{1, 0}, {1, 1}, {2, 1}, {1, 2}}};
    const CellArray flip{{{1, 0}, {0, 1}, {1, 1}, {2, 1}}};
    const CellArray left{{{1, 0}, {0, 1}, {1, 1}, {1, 2}}};
    CHECK(sorted(piece->cells(Rotation::Spawn)) == spawn);
    CHECK(sorted(piece->cells(Rotation::Right)) == right);
    CHECK(sorted(piece->cells(Rotation::Flip)) == flip);
    CHECK(sorted(piece->cells(Rotation::Left)) == left);
}

TEST_CASE("I piece is a flat bar that turns into a vertical bar", "[pieces]") {
    const auto piece = PieceFactory::create(PieceType::I);
    const CellArray spawn{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}};
    const CellArray right{{{2, 0}, {2, 1}, {2, 2}, {2, 3}}};
    const CellArray flip{{{0, 1}, {1, 1}, {2, 1}, {3, 1}}};
    const CellArray left{{{1, 0}, {1, 1}, {1, 2}, {1, 3}}};
    CHECK(sorted(piece->cells(Rotation::Spawn)) == spawn);
    CHECK(sorted(piece->cells(Rotation::Right)) == right);
    CHECK(sorted(piece->cells(Rotation::Flip)) == flip);
    CHECK(sorted(piece->cells(Rotation::Left)) == left);
}

TEST_CASE("S Z J L spawn shapes match the SRS diagrams", "[pieces]") {
    const CellArray s{{{0, 1}, {1, 1}, {1, 2}, {2, 2}}};
    const CellArray z{{{1, 1}, {2, 1}, {0, 2}, {1, 2}}};
    const CellArray j{{{0, 1}, {1, 1}, {2, 1}, {0, 2}}};
    const CellArray l{{{0, 1}, {1, 1}, {2, 1}, {2, 2}}};
    CHECK(sorted(PieceFactory::create(PieceType::S)->cells(Rotation::Spawn)) == s);
    CHECK(sorted(PieceFactory::create(PieceType::Z)->cells(Rotation::Spawn)) == z);
    CHECK(sorted(PieceFactory::create(PieceType::J)->cells(Rotation::Spawn)) == j);
    CHECK(sorted(PieceFactory::create(PieceType::L)->cells(Rotation::Spawn)) == l);
}

TEST_CASE("O piece looks the same in every rotation", "[pieces]") {
    const auto piece = PieceFactory::create(PieceType::O);
    const CellArray expected = sorted(piece->cells(Rotation::Spawn));
    for (std::size_t r = 1; r < 4; ++r) {
        CHECK(sorted(piece->cells(rotationAt(r))) == expected);
    }
}

TEST_CASE("pieces spawn inside the hidden rows and fit an empty board", "[pieces]") {
    const Board board;
    for (const PieceType type : kAllPieceTypes) {
        INFO("piece " << toChar(type));
        const auto piece = PieceFactory::create(type);
        CHECK(board.canPlace(*piece));
        for (const Vec2i cell : piece->worldCells()) {
            CHECK(cell.y >= kVisibleHeight);
            CHECK(cell.x >= 0);
            CHECK(cell.x < kBoardWidth);
        }
    }
}

TEST_CASE("clone is an independent copy of the same piece", "[pieces]") {
    auto original = PieceFactory::create(PieceType::J);
    original->setPosition(Vec2i{5, 7});
    original->setRotation(Rotation::Left);

    const auto copy = original->clone();
    const Vec2i expectedPosition{5, 7};
    CHECK(copy.get() != original.get());
    CHECK(copy->type() == PieceType::J);
    CHECK(copy->position() == expectedPosition);
    CHECK(copy->rotation() == Rotation::Left);
    CHECK(copy->cells(Rotation::Left) == original->cells(Rotation::Left));

    original->setPosition(Vec2i{0, 0});  // moving the original must not move the copy
    CHECK(copy->position() == expectedPosition);
}
