#pragma once
/**
 * @file GameOverScreen.hpp
 * @brief Game-over screen presenting final score, high-score banner, Retry and Menu options.
 */
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Screen displayed after top-out/game-over condition is reached.
 */
class GameOverScreen final : public Screen {
public:
    /**
     * @brief Constructs a game over screen with score and high score indicator.
     * @param host Application host context.
     * @param finalScore Final score achieved by the player.
     * @param isNewHighScore True if this run beat the previous top score.
     */
    GameOverScreen(IScreenHost& host, int finalScore, bool isNewHighScore);

    /**
     * @brief Handles Retry (R, Enter, Space) and Menu (M, Escape) controls.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Frame update hook (unused for static game over screen).
     */
    void update(float dt) override;

    /**
     * @brief Renders the game over banner, final score, and option buttons.
     */
    void draw(IRenderer& renderer) override;

    /**
     * @brief Returns recorded final score.
     */
    [[nodiscard]] int finalScore() const noexcept { return finalScore_; }

    /**
     * @brief Returns true if this run achieved a new top score.
     */
    [[nodiscard]] bool isNewHighScore() const noexcept { return isNewHighScore_; }

private:
    IScreenHost& host_;
    int finalScore_{0};
    bool isNewHighScore_{false};
};

}  // namespace tf::ui

