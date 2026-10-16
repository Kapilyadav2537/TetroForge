/**
 * @file test_scores.cpp
 * @brief High score management and file persistence tests.
 * @note SRS: FR-4.1, FR-4.2, §6.2 (ScoreManager, ScoreFileError), TC-07.
 */
#include <catch2/catch_test_macros.hpp>

#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include <tf/core/ScoreManager.hpp>

using tf::core::ScoreFileError;
using tf::core::ScoreManager;

namespace {

struct TempFileGuard {
    std::string path;
    explicit TempFileGuard(std::string p) : path(std::move(p)) {
        std::remove(path.c_str());
    }
    ~TempFileGuard() {
        std::remove(path.c_str());
    }
};

}  // namespace

TEST_CASE("ScoreManager starts with an empty list", "[scores]") {
    ScoreManager sm("temp_nonexistent.txt");
    CHECK(sm.top5().empty());
}

TEST_CASE("ScoreManager tracks and limits to top 5 scores in descending order", "[scores]") {
    ScoreManager sm("temp_test.txt");
    CHECK(sm.submit(100));
    CHECK(sm.submit(300));
    CHECK(sm.submit(200));
    CHECK(sm.submit(500));
    CHECK(sm.submit(400));
    CHECK(sm.submit(600));  // 6th score, should kick out 100

    const auto& top = sm.top5();
    REQUIRE(top.size() == 5U);
    CHECK(top[0] == 600);
    CHECK(top[1] == 500);
    CHECK(top[2] == 400);
    CHECK(top[3] == 300);
    CHECK(top[4] == 200);

    // Score lower than 5th is rejected
    CHECK_FALSE(sm.submit(50));
    CHECK(sm.top5().size() == 5U);
}

TEST_CASE("ScoreManager detects new high score correctly", "[scores]") {
    ScoreManager sm("temp_test.txt");
    CHECK(sm.isNewHighScore(100));
    sm.submit(100);
    CHECK_FALSE(sm.isNewHighScore(50));
    CHECK_FALSE(sm.isNewHighScore(100));
    CHECK(sm.isNewHighScore(150));
}

TEST_CASE("ScoreManager saves and reloads scores", "[scores]") {
    const TempFileGuard guard("temp_save_load.txt");
    {
        ScoreManager sm(guard.path);
        sm.submit(5000);
        sm.submit(2000);
        sm.submit(8000);
        sm.save();
    }
    {
        ScoreManager loaded(guard.path);
        loaded.load();
        const auto& top = loaded.top5();
        REQUIRE(top.size() == 3U);
        CHECK(top[0] == 8000);
        CHECK(top[1] == 5000);
        CHECK(top[2] == 2000);
    }
}

TEST_CASE("TC-07 missing highscores file is treated as an empty list without crashing", "[scores]") {
    const TempFileGuard guard("temp_missing_scores.txt");
    ScoreManager sm(guard.path);
    CHECK_NOTHROW(sm.load());
    CHECK(sm.top5().empty());
}

TEST_CASE("TC-07 load a corrupt highscores.txt yields empty list and no crash", "[scores]") {
    const TempFileGuard guard("temp_corrupt_scores.txt");
    {
        std::ofstream out(guard.path);
        out << "1000\ncorrupted_text\n500\n";
    }

    ScoreManager sm(guard.path);
    try {
        sm.load();
    } catch (const ScoreFileError& err) {
        CHECK(std::string(err.what()).find("ScoreManager::load") != std::string::npos);
    }
    CHECK(sm.top5().empty());
}

