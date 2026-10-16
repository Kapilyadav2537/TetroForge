#include <tf/ui/SoundPlayer.hpp>

#include <cmath>
#include <cstdint>
#include <vector>

namespace tf::ui {

SoundPlayer::SoundPlayer() = default;

#ifdef TF_HAS_SFML_AUDIO
namespace {

constexpr unsigned int kSampleRate = 44100U;
constexpr double kPi = 3.14159265358979323846;

/**
 * @brief Generates a frequency-swept mono 16-bit PCM waveform with an attack/decay envelope.
 * @param startFreq Beginning pitch in Hz.
 * @param endFreq Concluding pitch in Hz.
 * @param durationSeconds Total sound duration.
 * @param maxAmplitude Peak waveform height (up to 32767 for signed 16-bit PCM).
 */
std::vector<std::int16_t> generateSweep(double startFreq, double endFreq, double durationSeconds,
                                        double maxAmplitude) {
    // Step 1: Compute total discrete audio frames for this duration.
    const std::size_t sampleCount = static_cast<std::size_t>(static_cast<double>(kSampleRate) * durationSeconds);
    std::vector<std::int16_t> samples(sampleCount);

    double phase = 0.0;
    for (std::size_t i = 0; i < sampleCount; ++i) {
        // Step 2: Calculate instantaneous time and progress fraction [0.0..1.0].
        const double t = static_cast<double>(i) / static_cast<double>(kSampleRate);
        const double progress = t / durationSeconds;

        // Step 3: Linearly interpolate current frequency and accumulate continuous phase.
        const double freq = startFreq + (endFreq - startFreq) * progress;
        phase += 2.0 * kPi * freq / static_cast<double>(kSampleRate);

        // Step 4: Apply audio envelope (5% linear attack ramp to avoid clicks, 95% exponential decay).
        double envelope = 1.0;
        if (progress < 0.05) {
            envelope = progress / 0.05;
        } else {
            envelope = std::exp(-3.0 * (progress - 0.05));
        }

        // Step 5: Synthesize sine sample and quantize to 16-bit signed integer.
        const double sampleVal = maxAmplitude * envelope * std::sin(phase);
        samples[i] = static_cast<std::int16_t>(sampleVal);
    }
    return samples;
}

/**
 * @brief Generates a sequenced arpeggio of musical notes.
 * @param frequencies Array of note frequencies in Hz.
 * @param noteDuration Duration in seconds for each note.
 * @param maxAmplitude Peak volume height.
 */
std::vector<std::int16_t> generateArpeggio(const std::vector<double>& frequencies,
                                           double noteDuration, double maxAmplitude) {
    // Step 1: Compute buffer dimensions per note and total buffer size.
    const std::size_t noteSamples = static_cast<std::size_t>(static_cast<double>(kSampleRate) * noteDuration);
    const std::size_t totalSamples = noteSamples * frequencies.size();
    std::vector<std::int16_t> samples(totalSamples);

    // Step 2: Synthesize each note in sequence.
    for (std::size_t n = 0; n < frequencies.size(); ++n) {
        const double freq = frequencies[n];
        double phase = 0.0;
        for (std::size_t i = 0; i < noteSamples; ++i) {
            const double t = static_cast<double>(i) / static_cast<double>(kSampleRate);
            const double progress = t / noteDuration;
            phase += 2.0 * kPi * freq / static_cast<double>(kSampleRate);

            // Envelope: 5% attack, 95% exponential decay.
            double envelope = 1.0;
            if (progress < 0.05) {
                envelope = progress / 0.05;
            } else {
                envelope = std::exp(-2.5 * (progress - 0.05));
            }

            const double sampleVal = maxAmplitude * envelope * std::sin(phase);
            samples[n * noteSamples + i] = static_cast<std::int16_t>(sampleVal);
        }
    }
    return samples;
}

}  // namespace
#endif

void SoundPlayer::init() {
    if (initialized_) {
        return;
    }
#ifdef TF_HAS_SFML_AUDIO
    // Step 1: Synthesize Rotate sound: fast upward pitch sweep (550 Hz -> 850 Hz).
    const auto rotateSamples = generateSweep(550.0, 850.0, 0.045, 18000.0);
    rotateBuffer_.loadFromSamples(rotateSamples.data(), rotateSamples.size(), 1U, kSampleRate);

    // Step 2: Synthesize Lock sound: low downward thud sweep (180 Hz -> 100 Hz).
    const auto lockSamples = generateSweep(180.0, 100.0, 0.07, 22000.0);
    lockBuffer_.loadFromSamples(lockSamples.data(), lockSamples.size(), 1U, kSampleRate);

    // Step 3: Synthesize Line Clear sound: bright ascending C-major arpeggio (C5, E5, G5, C6).
    const std::vector<double> clearNotes{523.25, 659.25, 783.99, 1046.50};
    const auto clearSamples = generateArpeggio(clearNotes, 0.07, 20000.0);
    lineClearBuffer_.loadFromSamples(clearSamples.data(), clearSamples.size(), 1U, kSampleRate);

    // Step 4: Synthesize Game Over sound: descending minor cadence (G4, F4, Eb4, C4).
    const std::vector<double> overNotes{392.0, 349.23, 311.13, 261.63};
    const auto overSamples = generateArpeggio(overNotes, 0.12, 22000.0);
    gameOverBuffer_.loadFromSamples(overSamples.data(), overSamples.size(), 1U, kSampleRate);
#endif
    initialized_ = true;
}

void SoundPlayer::play(SoundEffect effect) {
    if (muted_ || !initialized_) {
        return;
    }
#ifdef TF_HAS_SFML_AUDIO
    // Select the appropriate buffer and trigger playback through the SFML sound instance.
    switch (effect) {
        case SoundEffect::Rotate:
            sound_.setBuffer(rotateBuffer_);
            sound_.play();
            break;
        case SoundEffect::Lock:
            sound_.setBuffer(lockBuffer_);
            sound_.play();
            break;
        case SoundEffect::LineClear:
            sound_.setBuffer(lineClearBuffer_);
            sound_.play();
            break;
        case SoundEffect::GameOver:
            sound_.setBuffer(gameOverBuffer_);
            sound_.play();
            break;
    }
#else
    static_cast<void>(effect);
#endif
}

}  // namespace tf::ui

