#pragma once
/**
 * @file MenuScreen.hpp
 * @brief Main menu screen with keyboard and mouse navigation.
 */
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>

namespace tf::ui {

/**
 * @brief Main menu screen providing options to Play, view High Scores, or Quit.
 */
class MenuScreen final : public Screen {
public:
    /**
     * @brief Constructs the menu screen attached to the parent host context.
     */
    explicit MenuScreen(IScreenHost& host);

    /**
     * @brief Processes keyboard arrows, Enter/Space, Escape, and mouse clicks.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Frame update hook (unused for static menu).
     */
    void update(float dt) override;

    /**
     * @brief Renders the menu background, title, and interactive buttons.
     */
    void draw(IRenderer& renderer) override;

    /**
     * @brief Returns the index of the currently highlighted menu item [0..2].
     */
    [[nodiscard]] int selection() const noexcept { return selected_; }

private:
    void activateSelection();

    IScreenHost& host_;
    int selected_{0};
};

}  // namespace tf::ui

