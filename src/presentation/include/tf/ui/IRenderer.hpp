#pragma once
/**
 * @file IRenderer.hpp
 * @brief Abstract interface for rendering the game, board, HUD, and text.
 * @note SRS: §6.2 (IRenderer), §6.3, §6.4 (runtime polymorphism, abstraction).
 */
#include <string_view>
#include <vector>

#include <tf/core/Board.hpp>
#include <tf/core/Tetromino.hpp>

namespace tf::ui {

/**
 * @brief Abstract renderer interface implemented by SFMLRenderer.
 */
class IRenderer {
public:
    virtual ~IRenderer() = default;

    /** @brief Clears the render target. */
    virtual void clear() = 0;

    /** @brief Flushes and displays the drawn frame on the window. */
    virtual void display() = 0;

    /** @brief Draws the 10x20 visible board cells. */
    virtual void drawBoard(const core::Board& board) = 0;

    /** @brief Draws a tetromino at its position, or as a ghost piece if isGhost is true. */
    virtual void drawPiece(const core::Tetromino& piece, bool isGhost = false) = 0;

    /** @brief Draws the HUD panel with score, level, lines, next piece, and optional held piece preview. */
    virtual void drawHud(int score, int level, int lines, const core::Tetromino& nextPiece,
                         const core::Tetromino* heldPiece = nullptr, bool canHold = true) = 0;

    /** @brief Draws text at (x, y) with the specified character size. */
    virtual void drawText(std::string_view text, float x, float y, unsigned int size) = 0;

    /** @brief Draws a pause overlay across the screen. */
    virtual void drawPauseOverlay() = 0;

    /** @brief Draws the main menu screen with selected item highlighted. */
    virtual void drawMenu(int selected) = 0;

    /** @brief Draws the game-over screen with score and optional high score banner. */
    virtual void drawGameOver(int score, bool isNewHighScore) = 0;

    /** @brief Draws the high score table displaying up to 5 scores. */
    virtual void drawHighScores(const std::vector<int>& scores) = 0;
};

}  // namespace tf::ui

