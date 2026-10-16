#pragma once
/**
 * @file Game.hpp
 * @brief Headless gameplay orchestration: actions, gravity, scoring and state.
 * @note SRS: FR-1.3--FR-2.5, §6.2 (Game), NFR-7.
 */
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <tf/core/Board.hpp>
#include <tf/core/Tetromino.hpp>
#include <tf/core/Types.hpp>

namespace tf::core {

/** @brief Actions accepted by the gameplay layer. */
enum class Action : std::uint8_t {
    MoveLeft,
    MoveRight,
    RotateCW,
    RotateCCW,
    SoftDrop,
    HardDrop,
    Hold,
    Pause,
};

/** @brief Lifecycle state of a game. */
enum class GameState : std::uint8_t { Playing, Paused, GameOver };

/** @brief Owns a board, the active/next pieces, RNG and gameplay counters. */
class Game {
public:
    /** @brief Starts a new deterministic game using @p seed for piece selection. */
    explicit Game(std::uint32_t seed = 0xC0FFEEU);

    /** @brief Applies one player action. */
    void apply(Action action);
    /** @brief Advances gravity by one row, locking and spawning when blocked. */
    void stepDown();
    /** @brief Resets the board, counters and piece sequence using the original seed. */
    void restart();

    /** @brief Read-only board access for renderers and tests. */
    [[nodiscard]] const Board& board() const noexcept { return board_; }
    /** @brief Mutable board access for testing setups. */
    [[nodiscard]] Board& board() noexcept { return board_; }
    /** @brief The currently falling piece, or the last piece after game over. */
    [[nodiscard]] const Tetromino& activePiece() const noexcept { return *active_; }
    /** @brief The piece that will spawn next. */
    [[nodiscard]] const Tetromino& nextPiece() const noexcept { return *next_; }
    /** @brief The held piece if any, or nullptr. */
    [[nodiscard]] const Tetromino* heldPiece() const noexcept { return held_.get(); }
    /** @brief True if the player is allowed to hold a piece this turn. */
    [[nodiscard]] bool canHold() const noexcept { return canHold_; }
    /** @brief Returns a clone of the active piece positioned where it would land. */
    [[nodiscard]] std::unique_ptr<Tetromino> ghostPiece() const;
    /** @brief Returns the landing position for the active piece. */
    [[nodiscard]] Vec2i ghostPosition() const;
    /** @brief Current lifecycle state. */
    [[nodiscard]] GameState state() const noexcept { return state_; }
    /** @brief Current score. */
    [[nodiscard]] int score() const noexcept { return score_; }
    /** @brief Current level, starting at one. */
    [[nodiscard]] int level() const noexcept { return lines_ / 10 + 1; }
    /** @brief Number of cleared lines. */
    [[nodiscard]] int lines() const noexcept { return lines_; }
    /** @brief Gravity interval in milliseconds for the current level. */
    [[nodiscard]] int fallIntervalMs() const noexcept;
    /** @brief Number of lines cleared during the last lock step. */
    [[nodiscard]] int lastClearedLines() const noexcept { return lastClearedLines_; }
    /** @brief True if the previous action caused the active piece to lock. */
    [[nodiscard]] bool lastActionLocked() const noexcept { return lastActionLocked_; }
    /** @brief Directly sets line count for deterministic test scenarios. */
    void setLines(int lines) noexcept { lines_ = lines; }
    /** @brief Directly sets score for test scenarios. */
    void setScore(int score) noexcept { score_ = score; }
    /** @brief Adds score corresponding to @p cleared lines at the current level. */
    void addScoreForLines(int cleared);

private:
    void refillBag();
    [[nodiscard]] PieceType randomType();
    void lockActive();
    void spawnNext();

    Board board_;
    std::uint32_t rngState_;
    std::uint32_t seed_;
    std::array<PieceType, kPieceTypeCount> bag_{};
    std::size_t bagIndex_{kPieceTypeCount};
    std::unique_ptr<Tetromino> active_;
    std::unique_ptr<Tetromino> next_;
    std::unique_ptr<Tetromino> held_{nullptr};
    GameState state_{GameState::Playing};
    int score_{0};
    int lines_{0};
    int lastClearedLines_{0};
    bool lastActionLocked_{false};
    bool canHold_{true};
};

}  // namespace tf::core