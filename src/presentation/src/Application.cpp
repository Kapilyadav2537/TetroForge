#include <tf/ui/Application.hpp>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include <tf/ui/MenuScreen.hpp>
#include <tf/ui/SFMLRenderer.hpp>

namespace tf::ui {

Application::Application() : scoreManager_("highscores.txt") {}

Application::~Application() = default;

void Application::changeScreen(std::unique_ptr<Screen> nextScreen) {
    // Store requested screen in pending slot. It will be swapped in at the top of the next loop iteration.
    nextScreen_ = std::move(nextScreen);
}

void Application::requestQuit() {
    isRunning_ = false;
    if (window_.isOpen()) {
        window_.close();
    }
}

int Application::run() {
    // Step 1: Create the main display window (800x720 pixels) and lock frame rate to 60 FPS.
    window_.create(sf::VideoMode(800U, 720U), "TetroForge");
    window_.setFramerateLimit(60U);

    // Step 2: Initialize sound synthesis buffers.
    soundPlayer_.init();

    // Step 3: Load existing high scores from disk (missing file is treated as an empty list).
    try {
        scoreManager_.load();
    } catch (const core::ScoreFileError&) {
        // Continue silently if file is corrupt or missing; new high scores will overwrite.
    }

    // Step 4: Create the 2D hardware renderer and launch the starting MenuScreen.
    renderer_ = std::make_unique<SFMLRenderer>(window_);
    currentScreen_ = std::make_unique<MenuScreen>(*this);

    // Step 5: Start the high-resolution frame clock.
    sf::Clock clock;

    // Step 6: Main application loop.
    while (window_.isOpen() && isRunning_) {
        // Step 6a: Calculate delta time elapsed since previous frame (in seconds).
        const float dt = clock.restart().asSeconds();

        // Step 6b: Poll and dispatch all queued OS and user input events.
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

        // Step 6c: Apply any deferred screen transition requested during event processing.
        if (nextScreen_) {
            currentScreen_ = std::move(nextScreen_);
        }

        // Step 6d: Update screen physics/state and draw current frame.
        if (currentScreen_ && window_.isOpen()) {
            currentScreen_->update(dt);
            currentScreen_->draw(*renderer_);
        }
    }

    return 0;
}

}  // namespace tf::ui