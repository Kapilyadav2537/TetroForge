#pragma once
/**
 * @file HighScoreScreen.hpp
 * @brief High-score leaderboard screen presenting the top 5 scores.
 */
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Screen displaying the top 5 recorded scores from persistent storage.
 */
class HighScoreScreen final : public Screen {
public:
    /**
     * @brief Constructs a high score screen attached to the application host.
     */
    explicit HighScoreScreen(IScreenHost& host);

    /**
     * @brief Returns to the main menu upon Escape, Enter, Space, M, or mouse click.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Frame update hook (unused for static leaderboard screen).
     */
    void update(float dt) override;

    /**
     * @brief Renders the high-scores table.
     */
    void draw(IRenderer& renderer) override;

private:
    IScreenHost& host_;
};

}  // namespace tf::ui

