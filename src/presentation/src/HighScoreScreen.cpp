#include <tf/ui/HighScoreScreen.hpp>

#include <tf/core/ScoreManager.hpp>
#include <tf/ui/MenuScreen.hpp>

namespace tf::ui {

HighScoreScreen::HighScoreScreen(IScreenHost& host) : host_(host) {}

void HighScoreScreen::handleEvent(const sf::Event& event) {
    // Step 1: Any standard exit keystroke (Escape, Enter, Space, M) returns to the main menu.
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Enter ||
            event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::M) {
            host_.changeScreen(std::make_unique<MenuScreen>(host_));
        }
    }
    // Step 2: Any mouse click returns to the main menu.
    else if (event.type == sf::Event::MouseButtonPressed &&
             event.mouseButton.button == sf::Mouse::Left) {
        host_.changeScreen(std::make_unique<MenuScreen>(host_));
    }
}

void HighScoreScreen::update(float /*dt*/) {}

void HighScoreScreen::draw(IRenderer& renderer) {
    // Step 1: Clear framebuffer.
    renderer.clear();

    // Step 2: Query the top 5 scores from the score manager and render the leaderboard table.
    renderer.drawHighScores(host_.scoreManager().top5());

    // Step 3: Present buffer to the display.
    renderer.display();
}

}  // namespace tf::ui

