/** @file InputHandler.cpp  @brief SFML-to-Core input mapping. */
#include <tf/ui/InputHandler.hpp>

namespace tf::ui {

std::optional<core::Action> InputHandler::actionFor(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed) {
        return std::nullopt;
    }
    switch (event.key.code) {
        case sf::Keyboard::Left: return core::Action::MoveLeft;
        case sf::Keyboard::Right: return core::Action::MoveRight;
        case sf::Keyboard::Up:
        case sf::Keyboard::X: return core::Action::RotateCW;
        case sf::Keyboard::Z:
        case sf::Keyboard::LControl: return core::Action::RotateCCW;
        case sf::Keyboard::C:
        case sf::Keyboard::LShift: return core::Action::Hold;
        case sf::Keyboard::Down: return core::Action::SoftDrop;
        case sf::Keyboard::Space: return core::Action::HardDrop;
        case sf::Keyboard::P:
        case sf::Keyboard::Escape: return core::Action::Pause;
        default: return std::nullopt;
    }
}

}  // namespace tf::ui