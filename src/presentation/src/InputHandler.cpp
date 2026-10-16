#include <tf/ui/InputHandler.hpp>

namespace tf::ui {

std::optional<core::Action> InputHandler::actionFor(const sf::Event& event) {
    // Step 1: Filter out all non-keypress events.
    if (event.type != sf::Event::KeyPressed) {
        return std::nullopt;
    }

    // Step 2: Map physical keyboard keys to canonical guideline gameplay actions.
    switch (event.key.code) {
        // Lateral movement:
        case sf::Keyboard::Left:    return core::Action::MoveLeft;
        case sf::Keyboard::Right:   return core::Action::MoveRight;

        // Clockwise rotation (Up arrow or X key):
        case sf::Keyboard::Up:
        case sf::Keyboard::X:       return core::Action::RotateCW;

        // Counter-clockwise rotation (Z key or Left Control):
        case sf::Keyboard::Z:
        case sf::Keyboard::LControl: return core::Action::RotateCCW;

        // Hold piece chamber (C key or Left Shift):
        case sf::Keyboard::C:
        case sf::Keyboard::LShift:  return core::Action::Hold;

        // Drops:
        case sf::Keyboard::Down:    return core::Action::SoftDrop;
        case sf::Keyboard::Space:   return core::Action::HardDrop;

        // Pause toggle:
        case sf::Keyboard::P:
        case sf::Keyboard::Escape:  return core::Action::Pause;

        // Unmapped key:
        default:                    return std::nullopt;
    }
}

}  // namespace tf::ui