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
    // Step 1: Note orientation before action to detect successful rotations.
    const auto rotationBefore = game_.activePiece().rotation();

    // Step 2: Apply action to core gameplay model.
    game_.apply(action);

    // Step 3: Trigger sound effects based on action outcome.
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

    // Step 4: Verify whether game reached a terminal state (game over).
    checkSoundsAndState();
}

void PlayScreen::checkSoundsAndState() {
    if (game_.state() == core::GameState::GameOver) {
        // Step 1: Play game over sound cue.
        host_.soundPlayer().play(SoundEffect::GameOver);

        // Step 2: Check whether final score beats current #1 high score.
        const bool isNewHigh = host_.scoreManager().isNewHighScore(game_.score());

        // Step 3: Record score and attempt to persist to disk.
        host_.scoreManager().submit(game_.score());
        try {
            host_.scoreManager().save();
        } catch (const core::ScoreFileError&) {
            // High score file write failures are non-fatal.
        }

        // Step 4: Transition to Game Over screen.
        host_.changeScreen(std::make_unique<GameOverScreen>(host_, game_.score(), isNewHigh));
    }
}

void PlayScreen::handleEvent(const sf::Event& event) {
    // Step 1: Handle paused state controls.
    if (game_.state() == core::GameState::Paused) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) {
                // Unpause game
                game_.apply(core::Action::Pause);
            } else if (event.key.code == sf::Keyboard::R) {
                // Restart current game session
                game_.restart();
                gravityTimer_ = 0.0F;
            } else if (event.key.code == sf::Keyboard::M) {
                // Return to main menu
                host_.changeScreen(std::make_unique<MenuScreen>(host_));
            }
        }
        return;
    }

    // Step 2: Handle gameplay action key inputs when game is actively running.
    if (game_.state() == core::GameState::Playing) {
        const auto action = InputHandler::actionFor(event);
        if (action.has_value()) {
            handleAction(*action);
        }
    }
}

void PlayScreen::update(float dt) {
    // Only accumulate gravity time while actively playing.
    if (game_.state() != core::GameState::Playing) {
        return;
    }

    // Step 1: Accumulate frame delta time in milliseconds.
    gravityTimer_ += dt * 1000.0F;

    // Step 2: When accumulated time exceeds current level's drop interval, advance gravity.
    if (gravityTimer_ >= static_cast<float>(game_.fallIntervalMs())) {
        gravityTimer_ = 0.0F;
        game_.stepDown();

        // Step 3: Trigger appropriate audio cues upon piece lock or line clear.
        if (game_.lastActionLocked()) {
            if (game_.lastClearedLines() > 0) {
                host_.soundPlayer().play(SoundEffect::LineClear);
            } else {
                host_.soundPlayer().play(SoundEffect::Lock);
            }
        }

        // Step 4: Verify whether gravity caused top-out / game over.
        checkSoundsAndState();
    }
}

void PlayScreen::draw(IRenderer& renderer) {
    // Step 1: Clear framebuffer.
    renderer.clear();

    // Step 2: Draw playfield grid matrix.
    renderer.drawBoard(game_.board());

    // Step 3: Draw landing preview ghost piece (translucent).
    const auto ghost = game_.ghostPiece();
    if (ghost) {
        renderer.drawPiece(*ghost, true);
    }

    // Step 4: Draw active falling tetromino (opaque).
    renderer.drawPiece(game_.activePiece(), false);

    // Step 5: Render sidebar heads-up display (HUD).
    renderer.drawHud(game_.score(), game_.level(), game_.lines(), game_.nextPiece(),
                     game_.heldPiece(), game_.canHold());

    // Step 6: If paused, draw dark modal overlay with resume/restart instructions.
    if (game_.state() == core::GameState::Paused) {
        renderer.drawPauseOverlay();
    }

    // Step 7: Present frame buffer to the window.
    renderer.display();
}

}  // namespace tf::ui

