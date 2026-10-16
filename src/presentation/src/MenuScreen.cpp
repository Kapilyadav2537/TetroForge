#include <tf/ui/MenuScreen.hpp>

#include <tf/ui/HighScoreScreen.hpp>
#include <tf/ui/PlayScreen.hpp>
#include <tf/ui/SoundPlayer.hpp>

namespace tf::ui {

MenuScreen::MenuScreen(IScreenHost& host) : host_(host) {}

void MenuScreen::activateSelection() {
    // Dispatch action based on highlighted menu item:
    // 0 = Start Game
    // 1 = High Scores Leaderboard
    // 2 = Quit Application
    if (selected_ == 0) {
        host_.changeScreen(std::make_unique<PlayScreen>(host_));
    } else if (selected_ == 1) {
        host_.changeScreen(std::make_unique<HighScoreScreen>(host_));
    } else if (selected_ == 2) {
        host_.requestQuit();
    }
}

void MenuScreen::handleEvent(const sf::Event& event) {
    // Step 1: Process keyboard inputs.
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up) {
            // Cycle upwards with modulo arithmetic.
            selected_ = (selected_ + 2) % 3;
            host_.soundPlayer().play(SoundEffect::Rotate);
        } else if (event.key.code == sf::Keyboard::Down) {
            // Cycle downwards with modulo arithmetic.
            selected_ = (selected_ + 1) % 3;
            host_.soundPlayer().play(SoundEffect::Rotate);
        } else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
            // Trigger currently selected action.
            activateSelection();
        } else if (event.key.code == sf::Keyboard::Escape) {
            // Quick exit to desktop.
            host_.requestQuit();
        }
    }
    // Step 2: Process mouse hover interaction.
    else if (event.type == sf::Event::MouseMoved) {
        if (event.mouseMove.y >= 240 && event.mouseMove.y < 440 &&
            event.mouseMove.x >= 240 && event.mouseMove.x <= 560) {
            const int hovered = (event.mouseMove.y - 240) / 62;
            if (hovered >= 0 && hovered < 3) {
                selected_ = hovered;
            }
        }
    }
    // Step 3: Process mouse click activation.
    else if (event.type == sf::Event::MouseButtonPressed &&
             event.mouseButton.button == sf::Mouse::Left) {
        if (event.mouseButton.y >= 240 && event.mouseButton.y < 440 &&
            event.mouseButton.x >= 240 && event.mouseButton.x <= 560) {
            const int clicked = (event.mouseButton.y - 240) / 62;
            if (clicked >= 0 && clicked < 3) {
                selected_ = clicked;
                activateSelection();
            }
        }
    }
}

void MenuScreen::update(float /*dt*/) {}

void MenuScreen::draw(IRenderer& renderer) {
    // Step 1: Clear the framebuffer.
    renderer.clear();

    // Step 2: Render menu title and buttons with the current selection highlight.
    renderer.drawMenu(selected_);

    // Step 3: Present buffer to the display window.
    renderer.display();
}

}  // namespace tf::ui

