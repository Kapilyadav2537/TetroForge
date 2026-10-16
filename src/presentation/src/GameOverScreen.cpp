/** @file GameOverScreen.cpp  @note SRS: FR-3.3, §6.2. */
#include <tf/ui/GameOverScreen.hpp>

#include <tf/ui/MenuScreen.hpp>
#include <tf/ui/PlayScreen.hpp>

namespace tf::ui {

GameOverScreen::GameOverScreen(IScreenHost& host, int finalScore, bool isNewHighScore)
    : host_(host), finalScore_(finalScore), isNewHighScore_(isNewHighScore) {}

void GameOverScreen::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R || event.key.code == sf::Keyboard::Space ||
            event.key.code == sf::Keyboard::Enter) {
            host_.changeScreen(std::make_unique<PlayScreen>(host_));
        } else if (event.key.code == sf::Keyboard::M || event.key.code == sf::Keyboard::Escape) {
            host_.changeScreen(std::make_unique<MenuScreen>(host_));
        }
    } else if (event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Left) {
        // Retry button region (y approx 380..420)
        if (event.mouseButton.y >= 370 && event.mouseButton.y <= 415 &&
            event.mouseButton.x >= 250 && event.mouseButton.x <= 550) {
            host_.changeScreen(std::make_unique<PlayScreen>(host_));
        }
        // Menu button region (y approx 430..470)
        else if (event.mouseButton.y >= 425 && event.mouseButton.y <= 470 &&
                 event.mouseButton.x >= 250 && event.mouseButton.x <= 550) {
            host_.changeScreen(std::make_unique<MenuScreen>(host_));
        }
    }
}

void GameOverScreen::update(float /*dt*/) {}

void GameOverScreen::draw(IRenderer& renderer) {
    renderer.clear();
    renderer.drawGameOver(finalScore_, isNewHighScore_);
    renderer.display();
}

}  // namespace tf::ui

