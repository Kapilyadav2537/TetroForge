#pragma once
/**
 * @file GameOverScreen.hpp
 * @brief Game-over screen with final score, high-score banner, Retry and Menu.
 * @note SRS: FR-3.3, §6.2 (GameOverScreen), §6.3, §6.6.
 */
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Screen shown when the game ends, offering retry or returning to menu.
 */
class GameOverScreen final : public Screen {
public:
    /** @brief Constructs a game over screen with score and high score indicator. */
    GameOverScreen(IScreenHost& host, int finalScore, bool isNewHighScore);

    /** @brief Handles retry, menu keys and mouse clicks. */
    void handleEvent(const sf::Event& event) override;

    /** @brief Updates game over animation if any. */
    void update(float dt) override;

    /** @brief Draws the game over display. */
    void draw(IRenderer& renderer) override;

    /** @brief Returns recorded final score. */
    [[nodiscard]] int finalScore() const noexcept { return finalScore_; }

    /** @brief Returns true if this run achieved a new top score. */
    [[nodiscard]] bool isNewHighScore() const noexcept { return isNewHighScore_; }

private:
    IScreenHost& host_;
    int finalScore_{0};
    bool isNewHighScore_{false};
};

}  // namespace tf::ui

