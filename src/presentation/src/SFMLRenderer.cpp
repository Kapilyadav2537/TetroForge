#include <tf/ui/SFMLRenderer.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <string>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>

namespace tf::ui {
namespace {

constexpr float kBoardX = 120.0F;
constexpr float kBoardY = 80.0F;
constexpr float kCell = 28.0F;

const sf::Color kBackground{18, 22, 32};
const sf::Color kPanel{31, 38, 53};
const sf::Color kBorder{60, 75, 105};
const sf::Color kTextWhite{235, 239, 247};
const sf::Color kTextGold{241, 201, 76};
const sf::Color kTextDim{150, 160, 180};

const std::array<sf::Color, 8> kColors{{
    sf::Color::Transparent,
    sf::Color{58, 196, 220},   // I: Cyan
    sf::Color{241, 201, 76},  // O: Yellow
    sf::Color{170, 92, 220},  // T: Purple
    sf::Color{82, 196, 112},  // S: Green
    sf::Color{220, 76, 94},   // Z: Red
    sf::Color{76, 110, 220},  // J: Blue
    sf::Color{235, 133, 62}   // L: Orange
}};

// Compact 5x7 bitmap font table for ASCII characters
struct Glyph5x7 {
    char ch;
    std::array<std::uint8_t, 7> rows;
};

constexpr std::array<Glyph5x7, 49> kGlyphs{{
    {' ', {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}},
    {'!', {{0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x04}}},
    {'*', {{0x00, 0x15, 0x0E, 0x1F, 0x0E, 0x15, 0x00}}},
    {'+', {{0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00}}},
    {'-', {{0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00}}},
    {'.', {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04}}},
    {'/', {{0x01, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00}}},
    {':', {{0x00, 0x04, 0x00, 0x00, 0x04, 0x00, 0x00}}},
    {'<', {{0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02}}},
    {'>', {{0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08}}},
    {'[', {{0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E}}},
    {']', {{0x0E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0E}}},
    {'#', {{0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A}}},
    {'0', {{0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E}}},
    {'1', {{0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E}}},
    {'2', {{0x0E, 0x11, 0x01, 0x06, 0x08, 0x10, 0x1F}}},
    {'3', {{0x1F, 0x02, 0x04, 0x06, 0x01, 0x11, 0x0E}}},
    {'4', {{0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02}}},
    {'5', {{0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E}}},
    {'6', {{0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E}}},
    {'7', {{0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08}}},
    {'8', {{0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E}}},
    {'9', {{0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C}}},
    {'A', {{0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}}},
    {'B', {{0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E}}},
    {'C', {{0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E}}},
    {'D', {{0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C}}},
    {'E', {{0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}}},
    {'F', {{0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10}}},
    {'G', {{0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F}}},
    {'H', {{0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}}},
    {'I', {{0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E}}},
    {'J', {{0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C}}},
    {'K', {{0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}}},
    {'L', {{0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F}}},
    {'M', {{0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11}}},
    {'N', {{0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}}},
    {'O', {{0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}}},
    {'P', {{0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10}}},
    {'Q', {{0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D}}},
    {'R', {{0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}}},
    {'S', {{0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E}}},
    {'T', {{0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04}}},
    {'U', {{0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E}}},
    {'V', {{0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04}}},
    {'W', {{0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11}}},
    {'X', {{0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11}}},
    {'Y', {{0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04}}},
    {'Z', {{0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F}}}
}};

const std::array<std::uint8_t, 7>* findGlyph(char c) {
    const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    for (const auto& glyph : kGlyphs) {
        if (glyph.ch == upper) {
            return &glyph.rows;
        }
    }
    return nullptr;
}

}  // namespace

SFMLRenderer::SFMLRenderer(sf::RenderWindow& window) : window_(window) {}

void SFMLRenderer::clear() {
    window_.clear(kBackground);
}

void SFMLRenderer::display() {
    window_.display();
}

void SFMLRenderer::drawBitmapChar(char c, float x, float y, float scale, sf::Color color) {
    const auto* rows = findGlyph(c);
    if (!rows) {
        return;
    }
    for (std::size_t r = 0; r < 7; ++r) {
        const std::uint8_t rowVal = (*rows)[r];
        for (std::size_t col = 0; col < 5; ++col) {
            if ((rowVal & (1U << (4U - col))) != 0) {
                sf::RectangleShape pixel({scale, scale});
                pixel.setPosition(x + static_cast<float>(col) * scale,
                                  y + static_cast<float>(r) * scale);
                pixel.setFillColor(color);
                window_.draw(pixel);
            }
        }
    }
}

void SFMLRenderer::drawBitmapText(std::string_view text, float x, float y, unsigned int size, sf::Color color) {
    const float scale = std::max(1.0F, static_cast<float>(size) / 7.0F);
    const float charAdvance = 6.0F * scale;
    float currentX = x;

    for (const char c : text) {
        drawBitmapChar(c, currentX, y, scale, color);
        currentX += charAdvance;
    }
}

void SFMLRenderer::drawText(std::string_view text, float x, float y, unsigned int size) {
    drawText(text, x, y, size, kTextWhite);
}

void SFMLRenderer::drawText(std::string_view text, float x, float y, unsigned int size, sf::Color color) {
    drawBitmapText(text, x, y, size, color);
}

void SFMLRenderer::drawBoard(const core::Board& board) {
    // Step 1: Draw the 10x20 playfield background receptacle and outer border.
    sf::RectangleShape boardBg(sf::Vector2f{static_cast<float>(core::kBoardWidth) * kCell,
                                            static_cast<float>(core::kVisibleHeight) * kCell});
    boardBg.setPosition(kBoardX, kBoardY);
    boardBg.setFillColor(kPanel);
    boardBg.setOutlineThickness(3.0F);
    boardBg.setOutlineColor(kBorder);
    window_.draw(boardBg);

    // Step 2: Iterate through all cells in the visible playfield [0..9] x [0..19].
    for (int y = 0; y < core::kVisibleHeight; ++y) {
        for (int x = 0; x < core::kBoardWidth; ++x) {
            const core::Cell cell = board.cellAt(x, y);
            if (cell != core::Cell::Empty) {
                // Step 3: Map board (x, y) to window screen coordinates (inverting Y because row 0 is floor).
                sf::RectangleShape block({kCell - 2.0F, kCell - 2.0F});
                block.setPosition(kBoardX + static_cast<float>(x) * kCell + 1.0F,
                                  kBoardY + static_cast<float>(core::kVisibleHeight - 1 - y) * kCell + 1.0F);
                block.setFillColor(kColors[static_cast<std::size_t>(cell)]);
                window_.draw(block);
            }
        }
    }
}

void SFMLRenderer::drawPiece(const core::Tetromino& piece, bool isGhost) {
    // Step 1: Select color palette entry (ghost pieces render with translucent alpha fill).
    const sf::Color color = isGhost
                                ? sf::Color{105, 115, 135, 90}
                                : kColors[static_cast<std::size_t>(core::toCell(piece.type()))];

    // Step 2: Render each of the 4 constituent blocks.
    for (const core::Vec2i cell : piece.worldCells()) {
        // Skip hidden upper buffer rows (y >= 20).
        if (cell.y < 0 || cell.y >= core::kVisibleHeight || cell.x < 0 || cell.x >= core::kBoardWidth) {
            continue;
        }
        sf::RectangleShape block({kCell - 2.0F, kCell - 2.0F});
        block.setPosition(kBoardX + static_cast<float>(cell.x) * kCell + 1.0F,
                          kBoardY + static_cast<float>(core::kVisibleHeight - 1 - cell.y) * kCell + 1.0F);
        block.setFillColor(color);
        if (isGhost) {
            block.setOutlineThickness(1.0F);
            block.setOutlineColor(sf::Color{160, 175, 200, 180});
        }
        window_.draw(block);
    }
}

void SFMLRenderer::drawHud(int score, int level, int lines, const core::Tetromino& nextPiece,
                           const core::Tetromino* heldPiece, bool canHold) {
    // Step 1: Draw title branding header.
    drawText("TETROFORGE", 120.0F, 24.0F, 28U, kTextWhite);

    // Step 2: Draw Hold Chamber box on the left margin.
    constexpr float kHoldX = 14.0F;
    constexpr float kHoldY = 96.0F;
    drawText("HOLD", kHoldX, kHoldY, 18U, kTextDim);

    sf::RectangleShape holdBox(sf::Vector2f{92.0F, 92.0F});
    holdBox.setPosition(kHoldX, kHoldY + 30.0F);
    holdBox.setFillColor(kPanel);
    holdBox.setOutlineThickness(2.0F);
    holdBox.setOutlineColor(kBorder);
    window_.draw(holdBox);

    // Step 3: Draw held piece inside the chamber if one has been held.
    if (heldPiece != nullptr) {
        constexpr float kHoldCell = 18.0F;
        const float holdOriginX = (heldPiece->type() == core::PieceType::O || heldPiece->type() == core::PieceType::I)
                                      ? kHoldX + 10.0F
                                      : kHoldX + 19.0F;
        const float holdOriginY = (heldPiece->type() == core::PieceType::I)
                                      ? kHoldY + 40.0F
                                      : kHoldY + 48.0F;

        // Gray out if hold cannot be used again this turn.
        const sf::Color holdColor = canHold
                                        ? kColors[static_cast<std::size_t>(core::toCell(heldPiece->type()))]
                                        : sf::Color{100, 105, 115, 160};

        for (const core::Vec2i cell : heldPiece->cells(core::Rotation::Spawn)) {
            sf::RectangleShape block({kHoldCell - 2.0F, kHoldCell - 2.0F});
            block.setPosition(holdOriginX + static_cast<float>(cell.x) * kHoldCell,
                              holdOriginY + static_cast<float>(3 - cell.y) * kHoldCell);
            block.setFillColor(holdColor);
            window_.draw(block);
        }
    }
    drawText("[C] HOLD", kHoldX, kHoldY + 130.0F, 12U, kTextDim);

    // Step 4: Draw Next Piece Preview box on the right sidebar.
    constexpr float kNextX = 450.0F;
    constexpr float kNextY = 96.0F;
    drawText("NEXT", kNextX, kNextY, 18U, kTextDim);

    sf::RectangleShape previewBox(sf::Vector2f{140.0F, 110.0F});
    previewBox.setPosition(kNextX, kNextY + 30.0F);
    previewBox.setFillColor(kPanel);
    previewBox.setOutlineThickness(2.0F);
    previewBox.setOutlineColor(kBorder);
    window_.draw(previewBox);

    // Draw preview centered in preview box
    constexpr float kPreviewCell = 22.0F;
    const float previewOriginX = (nextPiece.type() == core::PieceType::O || nextPiece.type() == core::PieceType::I)
                                     ? kNextX + 26.0F
                                     : kNextX + 37.0F;
    const float previewOriginY = (nextPiece.type() == core::PieceType::I)
                                     ? kNextY + 45.0F
                                     : kNextY + 50.0F;

    const sf::Color nextColor = kColors[static_cast<std::size_t>(core::toCell(nextPiece.type()))];
    for (const core::Vec2i cell : nextPiece.cells(core::Rotation::Spawn)) {
        sf::RectangleShape block({kPreviewCell - 2.0F, kPreviewCell - 2.0F});
        block.setPosition(previewOriginX + static_cast<float>(cell.x) * kPreviewCell,
                          previewOriginY + static_cast<float>(3 - cell.y) * kPreviewCell);
        block.setFillColor(nextColor);
        window_.draw(block);
    }

    // Stats
    drawText("SCORE", kNextX, 260.0F, 16U, kTextDim);
    drawText(std::to_string(score), kNextX, 285.0F, 22U, kTextWhite);

    drawText("LEVEL", kNextX, 330.0F, 16U, kTextDim);
    drawText(std::to_string(level), kNextX, 355.0F, 22U, kTextWhite);

    drawText("LINES", kNextX, 400.0F, 16U, kTextDim);
    drawText(std::to_string(lines), kNextX, 425.0F, 22U, kTextWhite);

    // Controls reminder
    drawText("P / ESC   PAUSE", kNextX, 490.0F, 12U, kTextDim);
    drawText("SPACE     HARD DROP", kNextX, 510.0F, 12U, kTextDim);
    drawText("DOWN      SOFT DROP", kNextX, 530.0F, 12U, kTextDim);
    drawText("UP / X    ROTATE CW", kNextX, 550.0F, 12U, kTextDim);
    drawText("Z / CTRL  ROTATE CCW", kNextX, 570.0F, 12U, kTextDim);
    drawText("C / SHIFT HOLD", kNextX, 590.0F, 12U, kTextDim);
    drawText("ARROWS    MOVE", kNextX, 610.0F, 12U, kTextDim);
}

void SFMLRenderer::drawPauseOverlay() {
    sf::RectangleShape shade(sf::Vector2f{800.0F, 720.0F});
    shade.setFillColor(sf::Color{0, 0, 0, 180});
    window_.draw(shade);

    drawText("PAUSED", 310.0F, 260.0F, 36U, kTextGold);
    drawText("P / ESC  RESUME", 300.0F, 360.0F, 18U, kTextWhite);
    drawText("R        RESTART", 300.0F, 400.0F, 18U, kTextWhite);
    drawText("M        MAIN MENU", 300.0F, 440.0F, 18U, kTextWhite);
}

void SFMLRenderer::drawMenu(int selected) {
    drawText("TETROFORGE", 210.0F, 110.0F, 42U, kTextWhite);
    drawText("Endless Marathon Edition", 270.0F, 175.0F, 16U, kTextDim);

    constexpr std::array<std::string_view, 3> kMenuLabels{{"PLAY", "HIGH SCORES", "QUIT"}};
    for (int i = 0; i < 3; ++i) {
        const bool isSel = (i == selected);
        const sf::Color color = isSel ? kTextGold : sf::Color{180, 190, 205};

        if (isSel) {
            sf::RectangleShape highlight({300.0F, 46.0F});
            highlight.setPosition(250.0F, 252.0F + static_cast<float>(i) * 62.0F);
            highlight.setFillColor(sf::Color{45, 55, 75});
            highlight.setOutlineThickness(1.0F);
            highlight.setOutlineColor(kTextGold);
            window_.draw(highlight);
        }

        const std::string text = isSel ? ("> " + std::string(kMenuLabels[static_cast<std::size_t>(i)]) + " <")
                                       : std::string(kMenuLabels[static_cast<std::size_t>(i)]);
        drawText(text, 280.0F, 265.0F + static_cast<float>(i) * 62.0F, 20U, color);
    }

    drawText("ARROWS / ENTER   OR CLICK", 240.0F, 520.0F, 15U, kTextDim);
}

void SFMLRenderer::drawGameOver(int score, bool isNewHighScore) {
    drawText("GAME OVER", 245.0F, 130.0F, 40U, sf::Color{230, 80, 90});

    if (isNewHighScore) {
        drawText("*** NEW HIGH SCORE! ***", 230.0F, 210.0F, 20U, kTextGold);
    }

    drawText("FINAL SCORE", 310.0F, 265.0F, 18U, kTextDim);
    drawText(std::to_string(score), 310.0F, 295.0F, 28U, kTextWhite);

    sf::RectangleShape retryBtn({240.0F, 44.0F});
    retryBtn.setPosition(280.0F, 375.0F);
    retryBtn.setFillColor(kPanel);
    retryBtn.setOutlineThickness(1.0F);
    retryBtn.setOutlineColor(kBorder);
    window_.draw(retryBtn);
    drawText("[R]  RETRY", 335.0F, 388.0F, 18U, kTextGold);

    sf::RectangleShape menuBtn({240.0F, 44.0F});
    menuBtn.setPosition(280.0F, 430.0F);
    menuBtn.setFillColor(kPanel);
    menuBtn.setOutlineThickness(1.0F);
    menuBtn.setOutlineColor(kBorder);
    window_.draw(menuBtn);
    drawText("[M]  MAIN MENU", 315.0F, 443.0F, 18U, kTextWhite);
}

void SFMLRenderer::drawHighScores(const std::vector<int>& scores) {
    drawText("HIGH SCORES", 240.0F, 100.0F, 38U, kTextWhite);
    drawText("TOP 5 RECORDS", 315.0F, 165.0F, 16U, kTextDim);

    if (scores.empty()) {
        drawText("NO SAVED SCORES YET", 260.0F, 270.0F, 18U, kTextDim);
        drawText("Play a game to set a record!", 250.0F, 310.0F, 16U, kTextDim);
    } else {
        const std::size_t count = std::min<std::size_t>(scores.size(), 5U);
        for (std::size_t i = 0; i < count; ++i) {
            const sf::Color color = (i == 0) ? kTextGold : kTextWhite;
            const std::string rank = "#" + std::to_string(i + 1) + "   ";
            const std::string scoreStr = std::to_string(scores[i]);

            drawText(rank, 300.0F, 230.0F + static_cast<float>(i) * 44.0F, 20U, color);
            drawText(scoreStr, 400.0F, 230.0F + static_cast<float>(i) * 44.0F, 20U, color);
        }
    }

    drawText("PRESS ESC / M TO RETURN", 255.0F, 520.0F, 16U, kTextDim);
}

}  // namespace tf::ui