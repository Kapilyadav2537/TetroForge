/** @file HighScoreScreen.cpp  @note SRS: FR-3.4, §6.2. */
#include <tf/ui/HighScoreScreen.hpp>

#include <tf/core/ScoreManager.hpp>
#include <tf/ui/MenuScreen.hpp>

namespace tf::ui {

HighScoreScreen::HighScoreScreen(IScreenHost& host) : host_(host) {}

void HighScoreScreen::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Enter ||
            event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::M) {
            host_.changeScreen(std::make_unique<MenuScreen>(host_));
        }
    } else if (event.type == sf::Event::MouseButtonPressed &&
               event.mouseButton.button == sf::Mouse::Left) {
        // Any click returns to menu
        host_.changeScreen(std::make_unique<MenuScreen>(host_));
    }
}

void HighScoreScreen::update(float /*dt*/) {}

void HighScoreScreen::draw(IRenderer& renderer) {
    renderer.clear();
    renderer.drawHighScores(host_.scoreManager().top5());
    renderer.display();
}

}  // namespace tf::ui

