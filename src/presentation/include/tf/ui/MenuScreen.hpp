#pragma once
/**
 * @file MenuScreen.hpp
 * @brief Main menu screen with keyboard and mouse navigation.
 * @note SRS: FR-3.1, §6.2 (MenuScreen), §6.3, §6.6.
 */
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Screen presenting the main menu (Play, High Scores, Quit).
 */
class MenuScreen final : public Screen {
public:
    /** @brief Constructs the menu screen attached to @p host. */
    explicit MenuScreen(IScreenHost& host);

    /** @brief Handles keyboard navigation and mouse clicks. */
    void handleEvent(const sf::Event& event) override;

    /** @brief Updates menu state. */
    void update(float dt) override;

    /** @brief Draws the main menu using @p renderer. */
    void draw(IRenderer& renderer) override;

    /** @brief Returns currently selected menu item index. */
    [[nodiscard]] int selection() const noexcept { return selected_; }

private:
    void activateSelection();

    IScreenHost& host_;
    int selected_{0};
};

}  // namespace tf::ui

