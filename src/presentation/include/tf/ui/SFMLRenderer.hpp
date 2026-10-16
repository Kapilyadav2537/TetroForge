#pragma once
/**
 * @file SFMLRenderer.hpp
 * @brief SFML-based implementation of the IRenderer interface.
 * @note SRS: §6.2 (SFMLRenderer, IRenderer), §6.3, §6.4.
 */
#include <string_view>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <tf/core/Board.hpp>
#include <tf/core/Tetromino.hpp>
#include <tf/ui/IRenderer.hpp>

namespace tf::ui {

/**
 * @brief Concrete renderer drawing TetroForge graphics on an SFML RenderWindow.
 */
class SFMLRenderer final : public IRenderer {
public:
    /** @brief Constructs a renderer attached to @p window. */
    explicit SFMLRenderer(sf::RenderWindow& window);

    /** @brief Clears the window using background color. */
    void clear() override;

    /** @brief Flushes and displays the rendered frame. */
    void display() override;

    /** @brief Draws the 10x20 visible board. */
    void drawBoard(const core::Board& board) override;

    /** @brief Draws a piece at its board position (or ghost piece). */
    void drawPiece(const core::Tetromino& piece, bool isGhost = false) override;

    /** @brief Draws HUD with score, level, lines, next piece preview, and optional held piece. */
    void drawHud(int score, int level, int lines, const core::Tetromino& nextPiece,
                 const core::Tetromino* heldPiece = nullptr, bool canHold = true) override;

    /** @brief Draws text with default white/silver color. */
    void drawText(std::string_view text, float x, float y, unsigned int size) override;

    /** @brief Draws text with custom color. */
    void drawText(std::string_view text, float x, float y, unsigned int size, sf::Color color);

    /** @brief Draws pause overlay over the playfield. */
    void drawPauseOverlay() override;

    /** @brief Draws main menu screen with selected item highlighted. */
    void drawMenu(int selected) override;

    /** @brief Draws game-over screen with score and optional high score banner. */
    void drawGameOver(int score, bool isNewHighScore) override;

    /** @brief Draws high scores screen listing top scores. */
    void drawHighScores(const std::vector<int>& scores) override;

private:
    void drawBitmapChar(char c, float x, float y, float scale, sf::Color color);
    void drawBitmapText(std::string_view text, float x, float y, unsigned int size, sf::Color color);

    sf::RenderWindow& window_;
};

}  // namespace tf::ui