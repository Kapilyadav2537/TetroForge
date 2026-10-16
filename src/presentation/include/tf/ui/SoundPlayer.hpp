#pragma once
/**
 * @file SoundPlayer.hpp
 * @brief Procedural sound effect synthesizer and playback engine.
 */
#include <array>
#include <cstdint>

#ifdef TF_HAS_SFML_AUDIO
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#endif

namespace tf::ui {

/**
 * @brief Categorical sound effect cues used across the gameplay experience.
 */
enum class SoundEffect : std::uint8_t {
    Rotate,
    Lock,
    LineClear,
    GameOver,
};

/**
 * @brief Procedural audio engine synthesizing sound effects in-memory using PCM sine wave mathematics.
 * Requires no external audio files or dependencies on disk.
 */
class SoundPlayer {
public:
    /**
     * @brief Constructs default uninitialized sound player.
     */
    SoundPlayer();

    /**
     * @brief Default destructor.
     */
    ~SoundPlayer() = default;

    /**
     * @brief Synthesizes PCM audio sample buffers for all sound effect cues.
     */
    void init();

    /**
     * @brief Plays the requested sound effect cue.
     */
    void play(SoundEffect effect);

    /**
     * @brief Mutes or un-mutes audio playback.
     */
    void setMuted(bool muted) noexcept { muted_ = muted; }

    /**
     * @brief Returns true if audio playback is currently muted.
     */
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

