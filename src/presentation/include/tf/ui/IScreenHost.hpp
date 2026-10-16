#pragma once
/**
 * @file IScreenHost.hpp
 * @brief Host interface providing screen switching and shared application services.
 */
#include <memory>

namespace tf::core {
class ScoreManager;
}

namespace tf::ui {

class Screen;
class SoundPlayer;

/**
 * @brief Host context interface implemented by Application.
 * Allows active screens to request state transitions, play audio, and access scores.
 */
class IScreenHost {
public:
    virtual ~IScreenHost() = default;

    /**
     * @brief Requests deferred transition to a new screen.
     */
    virtual void changeScreen(std::unique_ptr<Screen> nextScreen) = 0;

    /**
     * @brief Signals the application to cleanly shut down and close window.
     */
    virtual void requestQuit() = 0;

    /**
     * @brief Provides access to the application-wide score leaderboard manager.
     */
    [[nodiscard]] virtual core::ScoreManager& scoreManager() noexcept = 0;

    /**
     * @brief Provides access to the procedural audio synthesizer.
     */
    [[nodiscard]] virtual SoundPlayer& soundPlayer() noexcept = 0;
};

}  // namespace tf::ui

