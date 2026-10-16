#pragma once
/**
 * @file Game.hpp
 * @brief Core gameplay loop orchestration: user actions, gravity ticks, piece spawning, and scoring.
 */
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <tf/core/Board.hpp>
#include <tf/core/Tetromino.hpp>
#include <tf/core/Types.hpp>

namespace tf::core {

/**
 * @brief Discrete actions that mutate the game state.
 */
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

/**
 * @brief Game lifecycle status.
 */
enum class GameState : std::uint8_t {
    Playing,
    Paused,
    GameOver,
};

/**
 * @brief Coordinates the board, active piece, next piece queue, hold chamber, and score counters.
 *
 * This class is entirely headless and decoupled from GUI/audio rendering.
 */
class Game {
public:
    /**
     * @brief Constructs a new deterministic game session using a specific PRNG seed.
     * @param seed 32-bit seed for the 7-bag randomizer.
     */
    explicit Game(std::uint32_t seed = 0xC0FFEEU);

    /**
     * @brief Executes a single gameplay action (movement, rotation, hold, drop, pause).
     */
    void apply(Action action);

    /**
     * @brief Advances gravity by one row downward. Locks piece if blocked.
     */
    void stepDown();

    /**
     * @brief Resets the playfield, counters, and piece bag back to initial session state.
     */
    void restart();

    /**
     * @brief Immutable reference to the playfield board.
     */
    [[nodiscard]] const Board& board() const noexcept { return board_; }

    /**
     * @brief Mutable reference to the playfield board (used in test fixtures).
     */
    [[nodiscard]] Board& board() noexcept { return board_; }

    /**
     * @brief Current active falling tetromino.
     */
    [[nodiscard]] const Tetromino& activePiece() const noexcept { return *active_; }

    /**
     * @brief Upcoming tetromino queued in the preview window.
     */
    [[nodiscard]] const Tetromino& nextPiece() const noexcept { return *next_; }

    /**
     * @brief Currently held tetromino (or nullptr if hold chamber is empty).
     */
    [[nodiscard]] const Tetromino* heldPiece() const noexcept { return held_.get(); }

    /**
     * @brief Returns true if the player can use the hold action on the current piece.
     */
    [[nodiscard]] bool canHold() const noexcept { return canHold_; }

    /**
     * @brief Computes and returns a ghost clone of the active piece positioned at the landing point.
     */
    [[nodiscard]] std::unique_ptr<Tetromino> ghostPiece() const;

    /**
     * @brief Computes the bottom-most valid origin where the active piece lands.
     */
    [[nodiscard]] Vec2i ghostPosition() const;

    /**
     * @brief Returns the current lifecycle state (Playing, Paused, or GameOver).
     */
    [[nodiscard]] GameState state() const noexcept { return state_; }

    /**
     * @brief Total accumulated score.
     */
    [[nodiscard]] int score() const noexcept { return score_; }

    /**
     * @brief Current level (Level 1 + 1 level per 10 lines cleared).
     */
    [[nodiscard]] int level() const noexcept { return lines_ / 10 + 1; }

    /**
     * @brief Total number of lines cleared so far.
     */
    [[nodiscard]] int lines() const noexcept { return lines_; }

    /**
     * @brief Gravity step interval in milliseconds for the current level.
     */
    [[nodiscard]] int fallIntervalMs() const noexcept;

    /**
     * @brief Number of lines cleared during the most recent lock action.
     */
    [[nodiscard]] int lastClearedLines() const noexcept { return lastClearedLines_; }

    /**
     * @brief Indicates whether the most recent action caused the piece to lock into the board.
     */
    [[nodiscard]] bool lastActionLocked() const noexcept { return lastActionLocked_; }

    /**
     * @brief Overrides line count (used in testing setups).
     */
    void setLines(int lines) noexcept { lines_ = lines; }

    /**
     * @brief Overrides score (used in testing setups).
     */
    void setScore(int score) noexcept { score_ = score; }

    /**
     * @brief Awards score points for clearing lines at the current level multiplier.
     */
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