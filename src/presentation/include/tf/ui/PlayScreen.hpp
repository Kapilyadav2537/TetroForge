#pragma once
/**
 * @file PlayScreen.hpp
 * @brief Play screen with board, active piece, ghost, next piece, HUD and pause overlay.
 * @note SRS: FR-1.1--FR-2.5, FR-3.2, FR-3.5, §6.2 (PlayScreen), §6.3, §6.5.
 */
#include <tf/core/Game.hpp>
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Screen orchestrating active gameplay, user input, gravity and HUD rendering.
 */
class PlayScreen final : public Screen {
public:
    /** @brief Constructs a new play screen attached to @p host. */
    explicit PlayScreen(IScreenHost& host, std::uint32_t seed = 0xC0FFEEU);

    /** @brief Handles gameplay keystrokes and pause toggles. */
    void handleEvent(const sf::Event& event) override;

    /** @brief Steps gravity and checks game-over condition. */
    void update(float dt) override;

    /** @brief Renders the board, pieces, HUD and optional pause overlay. */
    void draw(IRenderer& renderer) override;

    /** @brief Access to current core game instance. */
    [[nodiscard]] const core::Game& game() const noexcept { return game_; }

private:
    void handleAction(core::Action action);
    void checkSoundsAndState();

    IScreenHost& host_;
    core::Game game_;
    float gravityTimer_{0.0F};
};

}  // namespace tf::ui

