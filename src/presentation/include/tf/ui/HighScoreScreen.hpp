#pragma once
/**
 * @file HighScoreScreen.hpp
 * @brief High-score screen listing the top 5 scores.
 * @note SRS: FR-3.4, §6.2 (HighScoreScreen), §6.3, §6.6.
 */
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Screen displaying the top 5 recorded high scores.
 */
class HighScoreScreen final : public Screen {
public:
    /** @brief Constructs a high score screen attached to @p host. */
    explicit HighScoreScreen(IScreenHost& host);

    /** @brief Handles return to menu upon Escape, Enter or click. */
    void handleEvent(const sf::Event& event) override;

    /** @brief Updates screen. */
    void update(float dt) override;

    /** @brief Renders the top 5 high scores table. */
    void draw(IRenderer& renderer) override;

private:
    IScreenHost& host_;
};

}  // namespace tf::ui

