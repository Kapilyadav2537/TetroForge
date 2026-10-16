#include <tf/core/Game.hpp>

#include <array>
#include <utility>

#include <tf/core/PieceFactory.hpp>

namespace tf::core {

Game::Game(std::uint32_t seed) : rngState_(seed), seed_(seed) {
    // Step 1: Spawn initial active and preview pieces from the 7-bag randomizer.
    active_ = PieceFactory::create(randomType());
    next_ = PieceFactory::create(randomType());

    // Step 2: Validate spawn location. If blocked immediately, trigger game over.
    if (!board_.canPlace(*active_)) {
        state_ = GameState::GameOver;
    }
}

void Game::apply(Action action) {
    // Reset frame transient flags.
    lastActionLocked_ = false;
    lastClearedLines_ = 0;

    // Step 1: Handle pause toggles regardless of whether the game is currently playing or paused.
    if (action == Action::Pause) {
        if (state_ == GameState::Playing) {
            state_ = GameState::Paused;
        } else if (state_ == GameState::Paused) {
            state_ = GameState::Playing;
        }
        return;
    }

    // Step 2: Guard against gameplay actions when not actively playing.
    if (state_ != GameState::Playing) {
        return;
    }

    // Step 3: Dispatch gameplay action.
    switch (action) {
        case Action::MoveLeft: {
            // Attempt horizontal translation by -1 on X.
            auto candidate = active_->clone();
            candidate->setPosition(candidate->position() + Vec2i{-1, 0});
            if (board_.canPlace(*candidate)) {
                active_ = std::move(candidate);
            }
            break;
        }
        case Action::MoveRight: {
            // Attempt horizontal translation by +1 on X.
            auto candidate = active_->clone();
            candidate->setPosition(candidate->position() + Vec2i{1, 0});
            if (board_.canPlace(*candidate)) {
                active_ = std::move(candidate);
            }
            break;
        }
        case Action::RotateCW:
            // Rotate clockwise using SRS wall kicks.
            static_cast<void>(board_.tryRotate(*active_, Spin::Clockwise));
            break;
        case Action::RotateCCW:
            // Rotate counter-clockwise using SRS wall kicks.
            static_cast<void>(board_.tryRotate(*active_, Spin::CounterClockwise));
            break;
        case Action::Hold: {
            // Step 3a: Hold action is allowed only once per piece drop cycle.
            if (!canHold_) {
                break;
            }
            canHold_ = false;

            // Step 3b: If a piece is already in the hold chamber, swap it with the active piece.
            // Otherwise, store active piece in hold and spawn the next preview piece.
            if (held_) {
                auto newActive = PieceFactory::create(held_->type());
                held_ = PieceFactory::create(active_->type());
                active_ = std::move(newActive);
            } else {
                held_ = PieceFactory::create(active_->type());
                active_ = std::move(next_);
                next_ = PieceFactory::create(randomType());
            }

            // Step 3c: Check if the swapped-in piece fits on the board; if blocked, game over.
            if (!board_.canPlace(*active_)) {
                state_ = GameState::GameOver;
            }
            break;
        }
        case Action::SoftDrop: {
            // Step 3d: Move down by 1 cell; reward 1 point if piece successfully moved down.
            const Vec2i before = active_->position();
            stepDown();
            if (state_ == GameState::Playing && active_->position().y < before.y) {
                ++score_;
            }
            break;
        }
        case Action::HardDrop: {
            // Step 3e: Drop active piece to the floor instantly.
            int distance = 0;
            while (board_.canPlace(*active_, active_->position() + Vec2i{0, -1},
                                   active_->rotation())) {
                active_->setPosition(active_->position() + Vec2i{0, -1});
                ++distance;
            }
            // Reward 2 points per cell dropped.
            score_ += distance * 2;
            // Instantly lock piece upon landing.
            lockActive();
            break;
        }
        case Action::Pause:
            break;
    }
}

void Game::stepDown() {
    lastActionLocked_ = false;
    lastClearedLines_ = 0;

    if (state_ != GameState::Playing) {
        return;
    }

    // Step 1: Check if the cell directly below the active piece is free.
    const Vec2i below = active_->position() + Vec2i{0, -1};
    if (board_.canPlace(*active_, below, active_->rotation())) {
        // Move active piece down by one row.
        active_->setPosition(below);
    } else {
        // Piece cannot descend further; lock it into the playfield grid.
        lockActive();
    }
}

void Game::restart() {
    // Step 1: Clear the board grid.
    board_.clear();

    // Step 2: Reset PRNG state back to initial seed.
    rngState_ = seed_;
    bagIndex_ = kPieceTypeCount;

    // Step 3: Reset hold chamber and status flags.
    held_.reset();
    canHold_ = true;

    // Step 4: Reset gameplay metrics and counters.
    score_ = 0;
    lines_ = 0;
    lastClearedLines_ = 0;
    lastActionLocked_ = false;
    state_ = GameState::Playing;

    // Step 5: Spawn initial pieces for the new session.
    active_ = PieceFactory::create(randomType());
    next_ = PieceFactory::create(randomType());
    if (!board_.canPlace(*active_)) {
        state_ = GameState::GameOver;
    }
}

int Game::fallIntervalMs() const noexcept {
    // Gravity speed increases per level: starts at 1000ms at Level 1, decreasing 90ms per level.
    // Clamped to a minimum speed floor of 100ms.
    const int interval = 1000 - 90 * (level() - 1);
    return interval < 100 ? 100 : interval;
}

Vec2i Game::ghostPosition() const {
    // Raycasts downward row-by-row until collision to find the lowest valid placement.
    Vec2i pos = active_->position();
    while (board_.canPlace(*active_, pos + Vec2i{0, -1}, active_->rotation())) {
        pos = pos + Vec2i{0, -1};
    }
    return pos;
}

std::unique_ptr<Tetromino> Game::ghostPiece() const {
    // Creates a visual clone of the active piece positioned at the landing point.
    auto ghost = active_->clone();
    ghost->setPosition(ghostPosition());
    return ghost;
}

void Game::refillBag() {
    // Step 1: Fill bag with one instance of each of the 7 standard tetromino shapes.
    bag_ = kAllPieceTypes;

    // Step 2: Shuffle the 7 pieces using the Fisher-Yates shuffle algorithm.
    // A deterministic Linear Congruential Generator (LCG) is used for repeatability.
    for (std::size_t i = kPieceTypeCount - 1; i > 0; --i) {
        rngState_ = rngState_ * 1664525U + 1013904223U;
        const std::size_t j =
            static_cast<std::size_t>((rngState_ >> 16U) % static_cast<unsigned>(i + 1));
        std::swap(bag_[i], bag_[j]);
    }
    bagIndex_ = 0;
}

PieceType Game::randomType() {
    // If all 7 pieces in the current bag have been dealt, refill and reshuffle a new bag.
    if (bagIndex_ >= kPieceTypeCount) {
        refillBag();
    }
    return bag_[bagIndex_++];
}

void Game::lockActive() {
    // Step 1: Stamp the active piece permanently into the board grid.
    board_.lock(*active_);

    // Step 2: Clear completed rows and update score & line counters.
    const int cleared = board_.clearLines();
    addScoreForLines(cleared);
    lines_ += cleared;
    lastClearedLines_ = cleared;
    lastActionLocked_ = true;

    // Step 3: Re-enable the hold action for the new turn.
    canHold_ = true;

    // Step 4: Promote preview piece to active piece and generate next preview piece.
    spawnNext();
}

void Game::spawnNext() {
    // Step 1: Move upcoming piece into the active slot.
    active_ = std::move(next_);

    // Step 2: Generate next piece from the 7-bag.
    next_ = PieceFactory::create(randomType());

    // Step 3: If newly spawned piece collides immediately, game over (top-out condition).
    if (!board_.canPlace(*active_)) {
        state_ = GameState::GameOver;
    }
}

void Game::addScoreForLines(int cleared) {
    // Standard guideline scoring: Single = 100, Double = 300, Triple = 500, Tetris (4) = 800.
    // Multiplied by current level.
    constexpr std::array<int, 5> kLineScores{{0, 100, 300, 500, 800}};
    score_ += kLineScores[static_cast<std::size_t>(cleared)] * level();
}

}  // namespace tf::core