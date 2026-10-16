/**
 * @file test_game.cpp
 * @brief Core Game lifecycle, scoring, levels, gravity and controls.
 * @note SRS: FR-1.3--FR-2.5, §7.2 (TC-03, TC-04, TC-05, TC-06, TC-08).
 */
#include <catch2/catch_test_macros.hpp>

#include <set>

#include <tf/core/Game.hpp>

#include "TestHelpers.hpp"

using namespace tf::core;
using namespace tf::test;

TEST_CASE("a game starts with active and next pieces", "[game]") {
    const Game game(25U);
    CHECK(game.state() == GameState::Playing);
    CHECK(game.activePiece().position().y >= kVisibleHeight - 1);
    CHECK(game.score() == 0);
    CHECK(game.level() == 1);
    CHECK(game.fallIntervalMs() == 1000);
}

TEST_CASE("TC-08 FR-1.11 pause prevents movement until resumed", "[game]") {
    Game game(25U);
    const Vec2i before = game.activePiece().position();
    game.apply(Action::Pause);
    game.stepDown();
    CHECK(game.activePiece().position() == before);
    CHECK(game.state() == GameState::Paused);

    // Further ticks while paused do not move the piece
    game.stepDown();
    CHECK(game.activePiece().position() == before);

    // Resume allows gravity again
    game.apply(Action::Pause);
    CHECK(game.state() == GameState::Playing);
    game.stepDown();
    CHECK(game.activePiece().position().y == before.y - 1);
}

TEST_CASE("TC-04 FR-1.6 FR-2.2 hard drop lands at ghost position and scores 2 per cell", "[game]") {
    Game game(25U);
    const Vec2i startPos = game.activePiece().position();
    const Vec2i ghostPos = game.ghostPosition();
    const int dropDistance = startPos.y - ghostPos.y;
    REQUIRE(dropDistance > 0);

    const int scoreBefore = game.score();
    game.apply(Action::HardDrop);

    // Hard drop adds 2 points per cell dropped
    CHECK(game.score() == scoreBefore + dropDistance * 2);
    CHECK(game.lastActionLocked());
}

TEST_CASE("TC-03 FR-2.1 clear 4 rows at level 2 increases score by 1600", "[game]") {
    Game game(25U);
    // At 10 lines cleared, the level becomes 2 (FR-2.3)
    game.setLines(10);
    REQUIRE(game.level() == 2);

    // Pre-fill columns 0..8 of rows 0..3 on the board
    for (int col = 0; col < 9; ++col) {
        lockVerticalI(game.board(), col, 0);
    }

    // Lock a vertical I into column 9, spanning rows 0..3 to clear all 4 rows
    lockVerticalI(game.board(), 9, 0);
    const int cleared = game.board().clearLines();
    REQUIRE(cleared == 4);

    const int scoreBefore = game.score();
    game.addScoreForLines(cleared);

    // Line clear scoring at level 2: 800 * 2 = 1600 (FR-2.1)
    CHECK(game.score() == scoreBefore + 1600);
}

TEST_CASE("TC-05 FR-1.10 spawn a piece onto a filled top row causes GameOver", "[game]") {
    Game game(25U);
    // Hard drop repeatedly without moving: the stack builds up at columns 3..6
    while (game.state() == GameState::Playing) {
        game.apply(Action::HardDrop);
    }
    CHECK(game.state() == GameState::GameOver);
}

TEST_CASE("TC-06 FR-2.3 FR-2.4 clear 10 lines reaches level 2 with fall interval 910 ms", "[game]") {
    Game game(25U);
    CHECK(game.level() == 1);
    CHECK(game.fallIntervalMs() == 1000);

    // Simulate clearing 10 lines
    game.setLines(10);
    CHECK(game.level() == 2);
    // Fall interval formula: max(100, 1000 - 90 * (level - 1)) = 1000 - 90 * 1 = 910 ms
    CHECK(game.fallIntervalMs() == 910);

    // Level 3 at 20 lines
    game.setLines(20);
    CHECK(game.level() == 3);
    CHECK(game.fallIntervalMs() == 820);
}

TEST_CASE("ghost piece matches active piece shape and landing position", "[game]") {
    const Game game(25U);
    const auto ghost = game.ghostPiece();
    REQUIRE(ghost != nullptr);
    CHECK(ghost->type() == game.activePiece().type());
    CHECK(ghost->rotation() == game.activePiece().rotation());
    CHECK(ghost->position() == game.ghostPosition());
}

TEST_CASE("7-bag randomizer yields all 7 unique pieces in every 7-piece cycle", "[game]") {
    Game game(42U);

    // First bag: active piece (1st) and the subsequent 6 drops
    std::set<PieceType> firstBag;
    firstBag.insert(game.activePiece().type());
    for (int i = 0; i < 6; ++i) {
        game.board().clear();
        game.apply(Action::HardDrop);
        firstBag.insert(game.activePiece().type());
    }
    CHECK(firstBag.size() == 7U);

    // Second bag: exactly the next 7 pieces spawned as active
    std::set<PieceType> secondBag;
    for (int i = 0; i < 7; ++i) {
        game.board().clear();
        game.apply(Action::HardDrop);
        secondBag.insert(game.activePiece().type());
    }
    CHECK(secondBag.size() == 7U);

    // Third bag: another 7 pieces spawned as active
    std::set<PieceType> thirdBag;
    for (int i = 0; i < 7; ++i) {
        game.board().clear();
        game.apply(Action::HardDrop);
        thirdBag.insert(game.activePiece().type());
    }
    CHECK(thirdBag.size() == 7U);
}

TEST_CASE("counter-clockwise rotation rotates piece CCW and cycles through SRS states", "[game]") {
    Game game(12345U);
    const Rotation initialRot = game.activePiece().rotation();
    CHECK(initialRot == Rotation::Spawn);

    game.apply(Action::RotateCCW);
    CHECK(game.activePiece().rotation() == Rotation::Left);

    game.apply(Action::RotateCCW);
    CHECK(game.activePiece().rotation() == Rotation::Flip);

    game.apply(Action::RotateCCW);
    CHECK(game.activePiece().rotation() == Rotation::Right);

    game.apply(Action::RotateCCW);
    CHECK(game.activePiece().rotation() == Rotation::Spawn);
}

TEST_CASE("hold piece stores active piece, swaps, and resets on lock", "[game]") {
    Game game(999U);
    CHECK(game.heldPiece() == nullptr);
    CHECK(game.canHold());

    const PieceType firstType = game.activePiece().type();
    const PieceType nextType = game.nextPiece().type();

    // First hold: active piece is stored into held, next piece becomes active
    game.apply(Action::Hold);
    REQUIRE(game.heldPiece() != nullptr);
    CHECK(game.heldPiece()->type() == firstType);
    CHECK(game.activePiece().type() == nextType);
    CHECK_FALSE(game.canHold());

    // Holding a second time in the same turn without locking is ignored
    const PieceType currentActive = game.activePiece().type();
    game.apply(Action::Hold);
    CHECK(game.activePiece().type() == currentActive);
    CHECK(game.heldPiece()->type() == firstType);
    CHECK_FALSE(game.canHold());

    // Locking the active piece re-enables hold
    game.board().clear();
    game.apply(Action::HardDrop);
    CHECK(game.canHold());

    // Second hold swaps active piece with held piece
    const PieceType thirdActive = game.activePiece().type();
    game.apply(Action::Hold);
    REQUIRE(game.heldPiece() != nullptr);
    CHECK(game.heldPiece()->type() == thirdActive);
    CHECK(game.activePiece().type() == firstType);
    CHECK_FALSE(game.canHold());
}