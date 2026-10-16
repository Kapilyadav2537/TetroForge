#pragma once
/**
 * @file Screen.hpp
 * @brief Abstract base class for all application screens.
 * @note SRS: §6.2 (Screen), §6.3 (inheritance), §6.4 (runtime polymorphism).
 */
#include <SFML/Window/Event.hpp>

#include <tf/ui/IRenderer.hpp>

namespace tf::ui {

/**
 * @brief Abstract base class representing a display screen in the game.
 */
class Screen {
public:
    virtual ~Screen() = default;

    /** @brief Handles a single window or keyboard event. */
    virtual void handleEvent(const sf::Event& event) = 0;

    /** @brief Updates the screen state over delta time @p dt (in seconds). */
    virtual void update(float dt) = 0;

    /** @brief Draws the screen elements using the provided renderer. */
    virtual void draw(IRenderer& renderer) = 0;
};

}  // namespace tf::ui

