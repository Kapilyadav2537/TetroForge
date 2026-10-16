#pragma once
/**
 * @file ScoreManager.hpp
 * @brief High-score tracking, ranking, and flat-file persistence.
 */
#include <stdexcept>
#include <string>
#include <vector>

namespace tf::core {

/**
 * @brief Exception thrown when score file operations (open, read, parse, write) fail.
 */
class ScoreFileError : public std::runtime_error {
public:
    explicit ScoreFileError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Manages the leaderboard of top 5 scores with persistence to disk.
 */
class ScoreManager {
public:
    /**
     * @brief Constructs a score manager targeting the specified file path.
     * @param filePath Path to the score persistence file (defaults to "highscores.txt").
     */
    explicit ScoreManager(std::string filePath = "highscores.txt");

    /**
     * @brief Loads and parses scores from disk into the top-5 list.
     * A non-existent file is treated as an empty leaderboard.
     * @throws ScoreFileError if the file contains invalid or corrupt numeric data.
     */
    void load();

    /**
     * @brief Writes the current top-5 scores to disk in descending order.
     * @throws ScoreFileError if writing to the target file fails.
     */
    void save() const;

    /**
     * @brief Submits a candidate score to the leaderboard.
     * Inserts the score and keeps the top 5 highest entries.
     * @param score Positive score value to submit.
     * @return true if the score qualified for the top 5; false otherwise.
     */
    bool submit(int score);

    /**
     * @brief Checks if a score strictly exceeds the current all-time #1 score.
     * @param score Candidate score to test.
     * @return true if score is greater than current top score or if list is empty.
     */
    [[nodiscard]] bool isNewHighScore(int score) const noexcept;

    /**
     * @brief Returns the top scores in descending order (maximum 5 items).
     */
    [[nodiscard]] const std::vector<int>& top5() const noexcept { return scores_; }

    /**
     * @brief Clears in-memory scores.
     */
    void clear() noexcept { scores_.clear(); }

    /**
     * @brief Returns the configured file path.
     */
    [[nodiscard]] const std::string& filePath() const noexcept { return filePath_; }

    /**
     * @brief Reconfigures the persistence file path.
     */
    void setFilePath(std::string filePath) { filePath_ = std::move(filePath); }

private:
    std::string filePath_;
    std::vector<int> scores_;
};

}  // namespace tf::core

