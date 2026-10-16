#pragma once
/**
 * @file SoundPlayer.hpp
 * @brief Sound effects for gameplay events.
 * @note SRS: FR-3.5 (sound effects for rotate, lock, line clear, game over).
 */
#include <array>
#include <cstdint>

#ifdef TF_HAS_SFML_AUDIO
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#endif

namespace tf::ui {

/** @brief Sound effect types supported by the game. */
enum class SoundEffect : std::uint8_t {
    Rotate,
    Lock,
    LineClear,
    GameOver,
};

/**
 * @brief Synthesizes and plays sound effects for the game.
 */
class SoundPlayer {
public:
    /** @brief Constructs a default sound player. */
    SoundPlayer();

    /** @brief Destroys the sound player. */
    ~SoundPlayer() = default;

    /** @brief Initializes synthesized audio buffers. */
    void init();

    /** @brief Plays the specified sound effect. */
    void play(SoundEffect effect);

    /** @brief Sets whether audio playback is muted. */
    void setMuted(bool muted) noexcept { muted_ = muted; }

    /** @brief Returns true if audio playback is currently muted. */
    [[nodiscard]] bool isMuted() const noexcept { return muted_; }

private:
    bool muted_{false};
    bool initialized_{false};

#ifdef TF_HAS_SFML_AUDIO
    sf::SoundBuffer rotateBuffer_;
    sf::SoundBuffer lockBuffer_;
    sf::SoundBuffer lineClearBuffer_;
    sf::SoundBuffer gameOverBuffer_;
    sf::Sound sound_;
#endif
};

}  // namespace tf::ui

