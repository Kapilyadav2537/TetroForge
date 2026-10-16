#pragma once
/**
 * @file InputHandler.hpp
 * @brief Maps platform-specific SFML keyboard events to core game Actions.
 */
#include <optional>

#include <SFML/Window/Event.hpp>

#include <tf/core/Game.hpp>

namespace tf::ui {

/**
 * @brief Translates raw SFML window/input events into domain-level gameplay actions.
 */
class InputHandler {
public:
    /**
     * @brief Evaluates an incoming window event and returns a corresponding gameplay action if mapped.
     * @param event Raw SFML event to evaluate.
     * @return Action enum wrapped in std::optional, or std::nullopt if unmapped.
     */
    [[nodiscard]] static std::optional<core::Action> actionFor(const sf::Event& event);
};

}  // namespace tf::ui