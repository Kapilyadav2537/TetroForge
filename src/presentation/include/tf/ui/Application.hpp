#pragma once
/**
 * @file Application.hpp
 * @brief Main application coordinator and SFML window lifecycle owner.
 * @note SRS: §6.2 (Application), §6.3, §6.4, §6.5 (game loop).
 */
#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>

#include <tf/core/ScoreManager.hpp>
#include <tf/ui/IRenderer.hpp>
#include <tf/ui/IScreenHost.hpp>
#include <tf/ui/Screen.hpp>
#include <tf/ui/SoundPlayer.hpp>

namespace tf::ui {

/**
 * @brief Application class owning the window, renderer, and active Screen.
 */
class Application final : public IScreenHost {
public:
    /** @brief Constructs a new Application instance. */
    Application();

    /** @brief Cleans up Application resources. */
    ~Application() override;

    /** @brief Opens the window and runs the main event and rendering loop. */
    int run();

    /** @brief Switches the active screen to @p nextScreen. */
    void changeScreen(std::unique_ptr<Screen> nextScreen) override;

    /** @brief Requests application termination. */
    void requestQuit() override;

    /** @brief Returns reference to application score manager. */
    [[nodiscard]] core::ScoreManager& scoreManager() noexcept override { return scoreManager_; }

    /** @brief Returns reference to application sound player. */
    [[nodiscard]] SoundPlayer& soundPlayer() noexcept override { return soundPlayer_; }

private:
    sf::RenderWindow window_;
    std::unique_ptr<IRenderer> renderer_;
    std::unique_ptr<Screen> currentScreen_;
    std::unique_ptr<Screen> nextScreen_;
    core::ScoreManager scoreManager_;
    SoundPlayer soundPlayer_;
    bool isRunning_{true};
};

}  // namespace tf::ui