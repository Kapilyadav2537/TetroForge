/** @file Game.cpp  @note SRS: FR-1.3--FR-2.5, NFR-7. */
#include <tf/core/Game.hpp>

#include <array>
#include <utility>

#include <tf/core/PieceFactory.hpp>

namespace tf::core {

Game::Game(std::uint32_t seed) : rngState_(seed), seed_(seed) {
    active_ = PieceFactory::create(randomType());
    next_ = PieceFactory::create(randomType());
    if (!board_.canPlace(*active_)) {
        state_ = GameState::GameOver;
    }
}

void Game::apply(Action action) {
    lastActionLocked_ = false;
    lastClearedLines_ = 0;

    if (action == Action::Pause) {
        if (state_ == GameState::Playing) {
            state_ = GameState::Paused;
        } else if (state_ == GameState::Paused) {
            state_ = GameState::Playing;
        }
        return;
    }
    if (state_ != GameState::Playing) {
        return;
    }

    switch (action) {
        case Action::MoveLeft: {
            auto candidate = active_->clone();
            candidate->setPosition(candidate->position() + Vec2i{-1, 0});
            if (board_.canPlace(*candidate)) {
                active_ = std::move(candidate);
            }
            break;
        }
        case Action::MoveRight: {
            auto candidate = active_->clone();
            candidate->setPosition(candidate->position() + Vec2i{1, 0});
            if (board_.canPlace(*candidate)) {
                active_ = std::move(candidate);
            }
            break;
        }
        case Action::RotateCW:
            static_cast<void>(board_.tryRotate(*active_, Spin::Clockwise));
            break;
        case Action::RotateCCW:
            static_cast<void>(board_.tryRotate(*active_, Spin::CounterClockwise));
            break;
        case Action::Hold: {
            if (!canHold_) {
                break;
            }
            canHold_ = false;
            if (held_) {
                auto newActive = PieceFactory::create(held_->type());
                held_ = PieceFactory::create(active_->type());
                active_ = std::move(newActive);
            } else {
                held_ = PieceFactory::create(active_->type());
                active_ = std::move(next_);
                next_ = PieceFactory::create(randomType());
            }
            if (!board_.canPlace(*active_)) {
                state_ = GameState::GameOver;
            }
            break;
        }
        case Action::SoftDrop: {
            const Vec2i before = active_->position();
            stepDown();
            if (state_ == GameState::Playing && active_->position().y < before.y) {
                ++score_;
            }
            break;
        }
        case Action::HardDrop: {
            int distance = 0;
            while (board_.canPlace(*active_, active_->position() + Vec2i{0, -1},
                                   active_->rotation())) {
                active_->setPosition(active_->position() + Vec2i{0, -1});
                ++distance;
            }
            score_ += distance * 2;
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
    const Vec2i below = active_->position() + Vec2i{0, -1};
    if (board_.canPlace(*active_, below, active_->rotation())) {
        active_->setPosition(below);
    } else {
        lockActive();
    }
}

void Game::restart() {
    board_.clear();
    rngState_ = seed_;
    bagIndex_ = kPieceTypeCount;
    held_.reset();
    canHold_ = true;
    score_ = 0;
    lines_ = 0;
    lastClearedLines_ = 0;
    lastActionLocked_ = false;
    state_ = GameState::Playing;
    active_ = PieceFactory::create(randomType());
    next_ = PieceFactory::create(randomType());
    if (!board_.canPlace(*active_)) {
        state_ = GameState::GameOver;
    }
}

int Game::fallIntervalMs() const noexcept {
    const int interval = 1000 - 90 * (level() - 1);
    return interval < 100 ? 100 : interval;
}

Vec2i Game::ghostPosition() const {
    Vec2i pos = active_->position();
    while (board_.canPlace(*active_, pos + Vec2i{0, -1}, active_->rotation())) {
        pos = pos + Vec2i{0, -1};
    }
    return pos;
}

std::unique_ptr<Tetromino> Game::ghostPiece() const {
    auto ghost = active_->clone();
    ghost->setPosition(ghostPosition());
    return ghost;
}

void Game::refillBag() {
    bag_ = kAllPieceTypes;
    for (std::size_t i = kPieceTypeCount - 1; i > 0; --i) {
        rngState_ = rngState_ * 1664525U + 1013904223U;
        const std::size_t j =
            static_cast<std::size_t>((rngState_ >> 16U) % static_cast<unsigned>(i + 1));
        std::swap(bag_[i], bag_[j]);
    }
    bagIndex_ = 0;
}

PieceType Game::randomType() {
    if (bagIndex_ >= kPieceTypeCount) {
        refillBag();
    }
    return bag_[bagIndex_++];
}

void Game::lockActive() {
    board_.lock(*active_);
    const int cleared = board_.clearLines();
    addScoreForLines(cleared);
    lines_ += cleared;
    lastClearedLines_ = cleared;
    lastActionLocked_ = true;
    canHold_ = true;
    spawnNext();
}

void Game::spawnNext() {
    active_ = std::move(next_);
    next_ = PieceFactory::create(randomType());
    if (!board_.canPlace(*active_)) {
        state_ = GameState::GameOver;
    }
}

void Game::addScoreForLines(int cleared) {
    constexpr std::array<int, 5> kLineScores{{0, 100, 300, 500, 800}};
    score_ += kLineScores[static_cast<std::size_t>(cleared)] * level();
}

}  // namespace tf::core