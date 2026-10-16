#pragma once
/**
 * @file IScreenHost.hpp
 * @brief Host interface providing screen switching and shared application services.
 * @note SRS: §6.2 (Application, Screens), §6.5, §6.6 (Screen flow).
 */
#include <memory>

namespace tf::core {
class ScoreManager;
}

namespace tf::ui {

class Screen;
class SoundPlayer;

/**
 * @brief Interface implemented by Application to host Screens.
 */
class IScreenHost {
public:
    virtual ~IScreenHost() = default;

    /** @brief Requests a transition to the next screen. */
    virtual void changeScreen(std::unique_ptr<Screen> nextScreen) = 0;

    /** @brief Requests application termination. */
    virtual void requestQuit() = 0;

    /** @brief Returns a reference to the application's score manager. */
    [[nodiscard]] virtual core::ScoreManager& scoreManager() noexcept = 0;

    /** @brief Returns a reference to the audio player for sound effects. */
    [[nodiscard]] virtual SoundPlayer& soundPlayer() noexcept = 0;
};

}  // namespace tf::ui

