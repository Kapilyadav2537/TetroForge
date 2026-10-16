#pragma once
/**
 * @file Screen.hpp
 * @brief Abstract base class for all application screens and visual states.
 */
#include <SFML/Window/Event.hpp>

#include <tf/ui/IRenderer.hpp>

namespace tf::ui {

/**
 * @brief Polymorphic base class representing a distinct UI state/screen (Menu, Play, GameOver, HighScores).
 */
class Screen {
public:
    virtual ~Screen() = default;

    /**
     * @brief Processes an incoming window or user input event.
     */
    virtual void handleEvent(const sf::Event& event) = 0;

    /**
     * @brief Updates internal animations, clocks, or game state over frame time delta @p dt (in seconds).
     */
    virtual void update(float dt) = 0;

    /**
     * @brief Renders the screen's visual elements using the provided abstract renderer.
     */
    virtual void draw(IRenderer& renderer) = 0;
};

}  // namespace tf::ui

