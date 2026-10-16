#pragma once
/**
 * @file Application.hpp
 * @brief Main application coordinator and SFML window lifecycle owner.
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
 * @brief Top-level application coordinator managing the SFML window, screen stack,
 * audio subsystem, and persistent high scores.
 */
class Application final : public IScreenHost {
public:
    /**
     * @brief Constructs Application instance and prepares subsystems.
     */
    Application();

    /**
     * @brief Cleans up Application resources upon shutdown.
     */
    ~Application() override;

    /**
     * @brief Opens the main render window and executes the primary game loop.
     * @return Process exit code (0 on normal termination).
     */
    int run();

    /**
     * @brief Queues a screen transition to take effect at the start of the next frame.
     */
    void changeScreen(std::unique_ptr<Screen> nextScreen) override;

    /**
     * @brief Requests clean termination of the application loop.
     */
    void requestQuit() override;

    /**
     * @brief Accessor for the persistent high-score manager.
     */
    [[nodiscard]] core::ScoreManager& scoreManager() noexcept override { return scoreManager_; }

    /**
     * @brief Accessor for the procedural audio synthesizer.
     */
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