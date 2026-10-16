/** @file Application.cpp  @note SRS: §6.2, §6.5 (game loop). */
#include <tf/ui/Application.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <tf/ui/MenuScreen.hpp>
#include <tf/ui/SFMLRenderer.hpp>

namespace tf::ui {

Application::Application() : scoreManager_("highscores.txt") {}

Application::~Application() = default;

void Application::changeScreen(std::unique_ptr<Screen> nextScreen) {
    nextScreen_ = std::move(nextScreen);
}

void Application::requestQuit() {
    isRunning_ = false;
    if (window_.isOpen()) {
        window_.close();
    }
}

int Application::run() {
    window_.create(sf::VideoMode(800U, 720U), "TetroForge");
    window_.setFramerateLimit(60U);

    soundPlayer_.init();

    try {
        scoreManager_.load();
    } catch (const core::ScoreFileError&) {
        // Missing or corrupt highscores file treated as empty list; never crash (FR-4.2)
    }

    renderer_ = std::make_unique<SFMLRenderer>(window_);
    currentScreen_ = std::make_unique<MenuScreen>(*this);

    sf::Clock clock;

    while (window_.isOpen() && isRunning_) {
        const float dt = clock.restart().asSeconds();

        sf::Event event{};
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
                break;
            }
            if (currentScreen_) {
                currentScreen_->handleEvent(event);
            }
        }

        if (nextScreen_) {
            currentScreen_ = std::move(nextScreen_);
        }

        if (currentScreen_ && window_.isOpen()) {
            currentScreen_->update(dt);
            currentScreen_->draw(*renderer_);
        }
    }

    return 0;
}

}  // namespace tf::ui