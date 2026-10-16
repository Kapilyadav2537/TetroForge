/** @file SoundPlayer.cpp  @note SRS: FR-3.5. */
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

std::vector<std::int16_t> generateSweep(double startFreq, double endFreq, double durationSeconds,
                                        double maxAmplitude) {
    const std::size_t sampleCount = static_cast<std::size_t>(static_cast<double>(kSampleRate) * durationSeconds);
    std::vector<std::int16_t> samples(sampleCount);

    double phase = 0.0;
    for (std::size_t i = 0; i < sampleCount; ++i) {
        const double t = static_cast<double>(i) / static_cast<double>(kSampleRate);
        const double progress = t / durationSeconds;
        const double freq = startFreq + (endFreq - startFreq) * progress;
        phase += 2.0 * kPi * freq / static_cast<double>(kSampleRate);

        // Envelope: linear attack 5%, exponential decay remaining 95%
        double envelope = 1.0;
        if (progress < 0.05) {
            envelope = progress / 0.05;
        } else {
            envelope = std::exp(-3.0 * (progress - 0.05));
        }

        const double sampleVal = maxAmplitude * envelope * std::sin(phase);
        samples[i] = static_cast<std::int16_t>(sampleVal);
    }
    return samples;
}

std::vector<std::int16_t> generateArpeggio(const std::vector<double>& frequencies,
                                           double noteDuration, double maxAmplitude) {
    const std::size_t noteSamples = static_cast<std::size_t>(static_cast<double>(kSampleRate) * noteDuration);
    const std::size_t totalSamples = noteSamples * frequencies.size();
    std::vector<std::int16_t> samples(totalSamples);

    for (std::size_t n = 0; n < frequencies.size(); ++n) {
        const double freq = frequencies[n];
        double phase = 0.0;
        for (std::size_t i = 0; i < noteSamples; ++i) {
            const double t = static_cast<double>(i) / static_cast<double>(kSampleRate);
            const double progress = t / noteDuration;
            phase += 2.0 * kPi * freq / static_cast<double>(kSampleRate);

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
    const auto rotateSamples = generateSweep(550.0, 850.0, 0.045, 18000.0);
    rotateBuffer_.loadFromSamples(rotateSamples.data(), rotateSamples.size(), 1U, kSampleRate);

    const auto lockSamples = generateSweep(180.0, 100.0, 0.07, 22000.0);
    lockBuffer_.loadFromSamples(lockSamples.data(), lockSamples.size(), 1U, kSampleRate);

    const std::vector<double> clearNotes{523.25, 659.25, 783.99, 1046.50};
    const auto clearSamples = generateArpeggio(clearNotes, 0.07, 20000.0);
    lineClearBuffer_.loadFromSamples(clearSamples.data(), clearSamples.size(), 1U, kSampleRate);

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

