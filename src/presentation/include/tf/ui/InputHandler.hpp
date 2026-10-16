#pragma once
/** @file InputHandler.hpp  @brief Maps SFML keyboard events to Core actions. */
#include <optional>

#include <SFML/Window/Event.hpp>

#include <tf/core/Game.hpp>

namespace tf::ui {

/** @brief Converts one SFML event into a gameplay action when applicable. */
class InputHandler {
public:
    /** @brief Returns the action represented by @p event, if any. */
    [[nodiscard]] static std::optional<core::Action> actionFor(const sf::Event& event);
};

}  // namespace tf::ui