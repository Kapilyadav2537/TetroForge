#pragma once
/**
 * @file ScoreManager.hpp
 * @brief High-score tracking and file persistence.
 * @note SRS: FR-4.1, FR-4.2, §6.1, §6.2 (ScoreManager, ScoreFileError), §6.4.
 */
#include <stdexcept>
#include <string>
#include <vector>

namespace tf::core {

/**
 * @brief Exception thrown when highscores.txt cannot be read, parsed or written.
 */
class ScoreFileError : public std::runtime_error {
public:
    /** @brief Constructs a score file error with an explanatory message. */
    explicit ScoreFileError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Manages the top 5 high scores with file persistence.
 */
class ScoreManager {
public:
    /** @brief Constructs a score manager using the specified file path. */
    explicit ScoreManager(std::string filePath = "highscores.txt");

    /**
     * @brief Loads the top 5 scores from the file.
     * @throws ScoreFileError if the file exists but contains invalid or corrupt data.
     */
    void load();

    /**
     * @brief Saves the current top 5 scores to the file.
     * @throws ScoreFileError if writing to the file fails.
     */
    void save() const;

    /**
     * @brief Submits a score and inserts it into the top 5 if high enough.
     * @return true if the score made it into the top 5.
     */
    bool submit(int score);

    /** @brief Returns true if the score exceeds the current #1 score. */
    [[nodiscard]] bool isNewHighScore(int score) const noexcept;

    /** @brief Returns the top 5 scores in descending order. */
    [[nodiscard]] const std::vector<int>& top5() const noexcept { return scores_; }

    /** @brief Clears all currently loaded scores. */
    void clear() noexcept { scores_.clear(); }

    /** @brief Returns the configured file path. */
    [[nodiscard]] const std::string& filePath() const noexcept { return filePath_; }

    /** @brief Sets a new file path for loading and saving. */
    void setFilePath(std::string filePath) { filePath_ = std::move(filePath); }

private:
    std::string filePath_;
    std::vector<int> scores_;
};

}  // namespace tf::core

