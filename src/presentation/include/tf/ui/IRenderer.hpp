#pragma once
/**
 * @file IRenderer.hpp
 * @brief Abstract interface defining all visual rendering operations.
 */
#include <string_view>
#include <vector>

#include <tf/core/Board.hpp>
#include <tf/core/Tetromino.hpp>

namespace tf::ui {

/**
 * @brief Pure abstract rendering contract implemented by hardware renderers (e.g., SFMLRenderer).
 * Completely decouples screens from concrete graphics libraries.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    /**
     * @brief Clears the rendering target with the default background color.
     */
    virtual void clear() = 0;

    /**
     * @brief Presents the backbuffer onto the display.
     */
    virtual void display() = 0;

    /**
     * @brief Renders the visible 10x20 playfield matrix and locked cells.
     */
    virtual void drawBoard(const core::Board& board) = 0;

    /**
     * @brief Renders a tetromino in world coordinates (either opaque or as a ghost piece).
     */
    virtual void drawPiece(const core::Tetromino& piece, bool isGhost = false) = 0;

    /**
     * @brief Renders the side HUD panel (score, level, lines, next preview, and hold chamber).
     */
    virtual void drawHud(int score, int level, int lines, const core::Tetromino& nextPiece,
                         const core::Tetromino* heldPiece = nullptr, bool canHold = true) = 0;

    /**
     * @brief Renders text string at specified coordinates.
     */
    virtual void drawText(std::string_view text, float x, float y, unsigned int size) = 0;

    /**
     * @brief Renders a semi-transparent pause overlay modal with resume options.
     */
    virtual void drawPauseOverlay() = 0;

    /**
     * @brief Renders the main menu title and interactive buttons.
     */
    virtual void drawMenu(int selected) = 0;

    /**
     * @brief Renders the game over banner, final score, and action buttons.
     */
    virtual void drawGameOver(int score, bool isNewHighScore) = 0;

    /**
     * @brief Renders the leaderboard ranking table for the top 5 scores.
     */
    virtual void drawHighScores(const std::vector<int>& scores) = 0;
};

}  // namespace tf::ui

