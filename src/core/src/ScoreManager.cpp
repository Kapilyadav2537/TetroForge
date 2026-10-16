#include <tf/core/ScoreManager.hpp>

#include <algorithm>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace tf::core {

ScoreManager::ScoreManager(std::string filePath) : filePath_(std::move(filePath)) {}

void ScoreManager::load() {
    // Step 1: Attempt to open the persistence file for reading.
    std::ifstream file(filePath_);
    if (!file.is_open()) {
        // If file does not exist yet, treat it as an empty leaderboard (clean first run).
        scores_.clear();
        return;
    }

    // Step 2: Read and parse each line.
    std::vector<int> loaded;
    std::string line;
    while (std::getline(file, line)) {
        // Step 2a: Trim whitespace characters.
        const auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            continue;  // Skip empty lines
        }
        const auto end = line.find_last_not_of(" \t\r\n");
        const std::string trimmed = line.substr(start, end - start + 1);

        // Step 2b: Parse integer string to value.
        std::size_t parsedLength = 0;
        int parsedValue = 0;
        try {
            parsedValue = std::stoi(trimmed, &parsedLength);
        } catch (...) {
            scores_.clear();
            throw ScoreFileError("ScoreManager::load: unparseable integer in " + filePath_);
        }

        // Step 2c: Validate that the entire string was consumed and the score is non-negative.
        if (parsedLength != trimmed.size() || parsedValue < 0) {
            scores_.clear();
            throw ScoreFileError("ScoreManager::load: corrupt score entry in " + filePath_);
        }
        loaded.push_back(parsedValue);
    }

    // Step 3: Check stream error flags.
    if (file.bad()) {
        scores_.clear();
        throw ScoreFileError("ScoreManager::load: I/O error reading " + filePath_);
    }

    // Step 4: Sort loaded scores in descending order and keep at most the top 5.
    std::sort(loaded.begin(), loaded.end(), std::greater<int>());
    if (loaded.size() > 5U) {
        loaded.resize(5U);
    }
    scores_ = std::move(loaded);
}

void ScoreManager::save() const {
    // Step 1: Open file for output (overwriting previous contents).
    std::ofstream file(filePath_);
    if (!file.is_open()) {
        throw ScoreFileError("ScoreManager::save: cannot open file for writing: " + filePath_);
    }

    // Step 2: Write out each score followed by a newline delimiter.
    for (const int score : scores_) {
        file << score << '\n';
    }

    // Step 3: Verify that stream operations succeeded.
    if (!file.good()) {
        throw ScoreFileError("ScoreManager::save: write error on " + filePath_);
    }
}

bool ScoreManager::submit(int score) {
    // Step 1: Ignore invalid or non-positive scores.
    if (score <= 0) {
        return false;
    }

    // Step 2: Insert candidate score into score list and sort descending.
    scores_.push_back(score);
    std::sort(scores_.begin(), scores_.end(), std::greater<int>());

    // Step 3: Determine if the newly inserted score placed within the top 5 ranks.
    bool inTop5 = false;
    const std::size_t limit = std::min<std::size_t>(scores_.size(), 5U);
    for (std::size_t i = 0; i < limit; ++i) {
        if (scores_[i] == score) {
            inTop5 = true;
            break;
        }
    }

    // Step 4: Discard scores beyond rank 5.
    if (scores_.size() > 5U) {
        scores_.resize(5U);
    }

    return inTop5;
}

bool ScoreManager::isNewHighScore(int score) const noexcept {
    // Step 1: A non-positive score is never a high score.
    if (score <= 0) {
        return false;
    }
    // Step 2: If the leaderboard is currently empty, any valid score is the new #1.
    if (scores_.empty()) {
        return true;
    }
    // Step 3: Test if the score strictly beats the current rank #1 score.
    return score > scores_.front();
}

}  // namespace tf::core

