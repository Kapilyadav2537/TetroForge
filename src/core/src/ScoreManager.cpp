/** @file ScoreManager.cpp  @note SRS: FR-4.1, FR-4.2, §6.2. */
#include <tf/core/ScoreManager.hpp>

#include <algorithm>
#include <fstream>
#include <functional>
#include <string>
#include <vector>

namespace tf::core {

ScoreManager::ScoreManager(std::string filePath) : filePath_(std::move(filePath)) {}

void ScoreManager::load() {
    std::ifstream file(filePath_);
    if (!file.is_open()) {
        // Missing file is treated as an empty list (FR-4.2)
        scores_.clear();
        return;
    }

    std::vector<int> loaded;
    std::string line;
    while (std::getline(file, line)) {
        const auto start = line.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            continue;
        }
        const auto end = line.find_last_not_of(" \t\r\n");
        const std::string trimmed = line.substr(start, end - start + 1);

        std::size_t parsedLength = 0;
        int parsedValue = 0;
        try {
            parsedValue = std::stoi(trimmed, &parsedLength);
        } catch (...) {
            scores_.clear();
            throw ScoreFileError("ScoreManager::load: unparseable integer in " + filePath_);
        }

        if (parsedLength != trimmed.size() || parsedValue < 0) {
            scores_.clear();
            throw ScoreFileError("ScoreManager::load: corrupt score entry in " + filePath_);
        }
        loaded.push_back(parsedValue);
    }

    if (file.bad()) {
        scores_.clear();
        throw ScoreFileError("ScoreManager::load: I/O error reading " + filePath_);
    }

    std::sort(loaded.begin(), loaded.end(), std::greater<int>());
    if (loaded.size() > 5U) {
        loaded.resize(5U);
    }
    scores_ = std::move(loaded);
}

void ScoreManager::save() const {
    std::ofstream file(filePath_);
    if (!file.is_open()) {
        throw ScoreFileError("ScoreManager::save: cannot open file for writing: " + filePath_);
    }

    for (const int score : scores_) {
        file << score << '\n';
    }

    if (!file.good()) {
        throw ScoreFileError("ScoreManager::save: write error on " + filePath_);
    }
}

bool ScoreManager::submit(int score) {
    if (score <= 0) {
        return false;
    }

    scores_.push_back(score);
    std::sort(scores_.begin(), scores_.end(), std::greater<int>());

    bool inTop5 = false;
    const std::size_t limit = std::min<std::size_t>(scores_.size(), 5U);
    for (std::size_t i = 0; i < limit; ++i) {
        if (scores_[i] == score) {
            inTop5 = true;
            break;
        }
    }

    if (scores_.size() > 5U) {
        scores_.resize(5U);
    }

    return inTop5;
}

bool ScoreManager::isNewHighScore(int score) const noexcept {
    if (score <= 0) {
        return false;
    }
    if (scores_.empty()) {
        return true;
    }
    return score > scores_.front();
}

}  // namespace tf::core

