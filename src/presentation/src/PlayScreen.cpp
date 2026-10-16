/** @file PlayScreen.cpp  @note SRS: FR-1.1--FR-2.5, FR-3.2, FR-3.5, §6.2. */
#include <tf/ui/PlayScreen.hpp>

#include <tf/core/ScoreManager.hpp>
#include <tf/ui/GameOverScreen.hpp>
#include <tf/ui/InputHandler.hpp>
#include <tf/ui/MenuScreen.hpp>
#include <tf/ui/SoundPlayer.hpp>

namespace tf::ui {

PlayScreen::PlayScreen(IScreenHost& host, std::uint32_t seed)
    : host_(host), game_(seed) {}

void PlayScreen::handleAction(core::Action action) {
    const auto rotationBefore = game_.activePiece().rotation();
    game_.apply(action);

    if ((action == core::Action::RotateCW || action == core::Action::RotateCCW) &&
        game_.activePiece().rotation() != rotationBefore) {
        host_.soundPlayer().play(SoundEffect::Rotate);
    }

    if (game_.lastActionLocked()) {
        if (game_.lastClearedLines() > 0) {
            host_.soundPlayer().play(SoundEffect::LineClear);
        } else {
            host_.soundPlayer().play(SoundEffect::Lock);
        }
    }

    checkSoundsAndState();
}

void PlayScreen::checkSoundsAndState() {
    if (game_.state() == core::GameState::GameOver) {
        host_.soundPlayer().play(SoundEffect::GameOver);
        const bool isNewHigh = host_.scoreManager().isNewHighScore(game_.score());
        host_.scoreManager().submit(game_.score());
        try {
            host_.scoreManager().save();
        } catch (const core::ScoreFileError&) {
            // Failures to save score are non-fatal (NFR-2)
        }
        host_.changeScreen(std::make_unique<GameOverScreen>(host_, game_.score(), isNewHigh));
    }
}

void PlayScreen::handleEvent(const sf::Event& event) {
    if (game_.state() == core::GameState::Paused) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) {
                game_.apply(core::Action::Pause);
            } else if (event.key.code == sf::Keyboard::R) {
                game_.restart();
                gravityTimer_ = 0.0F;
            } else if (event.key.code == sf::Keyboard::M) {
                host_.changeScreen(std::make_unique<MenuScreen>(host_));
            }
        }
        return;
    }

    if (game_.state() == core::GameState::Playing) {
        const auto action = InputHandler::actionFor(event);
        if (action.has_value()) {
            handleAction(*action);
        }
    }
}

void PlayScreen::update(float dt) {
    if (game_.state() != core::GameState::Playing) {
        return;
    }

    gravityTimer_ += dt * 1000.0F;
    if (gravityTimer_ >= static_cast<float>(game_.fallIntervalMs())) {
        gravityTimer_ = 0.0F;
        game_.stepDown();
        if (game_.lastActionLocked()) {
            if (game_.lastClearedLines() > 0) {
                host_.soundPlayer().play(SoundEffect::LineClear);
            } else {
                host_.soundPlayer().play(SoundEffect::Lock);
            }
        }
        checkSoundsAndState();
    }
}

void PlayScreen::draw(IRenderer& renderer) {
    renderer.clear();
    renderer.drawBoard(game_.board());

    const auto ghost = game_.ghostPiece();
    if (ghost) {
        renderer.drawPiece(*ghost, true);
    }
    renderer.drawPiece(game_.activePiece(), false);
    renderer.drawHud(game_.score(), game_.level(), game_.lines(), game_.nextPiece(),
                     game_.heldPiece(), game_.canHold());

    if (game_.state() == core::GameState::Paused) {
        renderer.drawPauseOverlay();
    }
    renderer.display();
}

}  // namespace tf::ui

