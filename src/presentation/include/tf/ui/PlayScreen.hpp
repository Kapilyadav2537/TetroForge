#pragma once
/**
 * @file PlayScreen.hpp
 * @brief Play screen orchestrating board, active piece, ghost piece, HUD, and pause overlay.
 */
#include <tf/core/Game.hpp>
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Active gameplay screen handling keyboard input, gravity tick timers,
 * audio cue triggers, and board/HUD rendering.
 */
class PlayScreen final : public Screen {
public:
    /**
     * @brief Constructs a new play screen attached to the parent host.
     * @param host Application host context.
     * @param seed Random seed for the piece generator.
     */
    explicit PlayScreen(IScreenHost& host, std::uint32_t seed = 0xC0FFEEU);

    /**
     * @brief Translates user keystrokes to gameplay actions or pause controls.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Accumulates delta time and triggers downward gravity ticks.
     */
    void update(float dt) override;

    /**
     * @brief Renders the playfield, pieces, HUD panel, and optional pause overlay.
     */
    void draw(IRenderer& renderer) override;

    /**
     * @brief Accessor to the core headless Game instance.
     */
    [[nodiscard]] const core::Game& game() const noexcept { return game_; }

private:
    void handleAction(core::Action action);
    void checkSoundsAndState();

    IScreenHost& host_;
    core::Game game_;
    float gravityTimer_{0.0F};
};

}  // namespace tf::ui

