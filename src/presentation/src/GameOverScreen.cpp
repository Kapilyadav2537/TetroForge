#include <tf/ui/GameOverScreen.hpp>

#include <tf/ui/MenuScreen.hpp>
#include <tf/ui/PlayScreen.hpp>

namespace tf::ui {

GameOverScreen::GameOverScreen(IScreenHost& host, int finalScore, bool isNewHighScore)
    : host_(host), finalScore_(finalScore), isNewHighScore_(isNewHighScore) {}

void GameOverScreen::handleEvent(const sf::Event& event) {
    // Step 1: Handle keyboard shortcuts.
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R || event.key.code == sf::Keyboard::Space ||
            event.key.code == sf::Keyboard::Enter) {
            // Immediate retry / start new game session.
            host_.changeScreen(std::make_unique<PlayScreen>(host_));
        } else if (event.key.code == sf::Keyboard::M || event.key.code == sf::Keyboard::Escape) {
            // Return to main menu.
            host_.changeScreen(std::make_unique<MenuScreen>(host_));
        }
    }
    // Step 2: Handle mouse button clicks on screen buttons.
    else if (event.type == sf::Event::MouseButtonPressed &&
             event.mouseButton.button == sf::Mouse::Left) {
        // Retry button click detection (y: 370..415, x: 250..550).
        if (event.mouseButton.y >= 370 && event.mouseButton.y <= 415 &&
            event.mouseButton.x >= 250 && event.mouseButton.x <= 550) {
            host_.changeScreen(std::make_unique<PlayScreen>(host_));
        }
        // Menu button click detection (y: 425..470, x: 250..550).
        else if (event.mouseButton.y >= 425 && event.mouseButton.y <= 470 &&
                 event.mouseButton.x >= 250 && event.mouseButton.x <= 550) {
            host_.changeScreen(std::make_unique<MenuScreen>(host_));
        }
    }
}

void GameOverScreen::update(float /*dt*/) {}

void GameOverScreen::draw(IRenderer& renderer) {
    // Step 1: Clear framebuffer.
    renderer.clear();

    // Step 2: Render game over header, final score, new high score tag, and navigation buttons.
    renderer.drawGameOver(finalScore_, isNewHighScore_);

    // Step 3: Present buffer to the display.
    renderer.display();
}

}  // namespace tf::ui

