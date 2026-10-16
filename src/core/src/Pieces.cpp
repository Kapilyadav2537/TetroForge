/**
 * @file Pieces.cpp
 * @brief Shape and kick data for the seven pieces.
 * @note SRS: FR-1.2, FR-1.4, §7.6 (constexpr shape tables), NFR-3 (integers only).
 *
 * Coordinates: x right, y UP, origin = bottom-left of the piece's bounding box.
 * Only the Spawn shape is typed in by hand. The other three rotations are
 * generated at compile time with the rule  (x, y) -> (y, boxSize - 1 - x),
 * which is a clockwise quarter turn inside the box.
 * Kick data is from the published SRS tables (Tetris Guideline / Tetris Wiki).
 */
#include <tf/core/Pieces.hpp>

#include <array>
#include <cstddef>
#include <memory>

namespace tf::core {
namespace {

using RotationTable = std::array<CellArray, 4>;

// Spawn origin: 3-wide pieces and O occupy rows 20-21, I occupies row 20 (Guideline).
constexpr Vec2i kSpawnDefault{3, 19};
constexpr Vec2i kSpawnI{3, 18};

constexpr CellArray rotateCwOnce(const CellArray& cells, int boxSize) noexcept {
    CellArray result{};
    for (std::size_t i = 0; i < cells.size(); ++i) {
        result[i] = Vec2i{cells[i].y, boxSize - 1 - cells[i].x};
    }
    return result;
}

constexpr RotationTable makeRotations(const CellArray& spawnShape, int boxSize) noexcept {
    RotationTable table{};
    table[0] = spawnShape;
    for (std::size_t i = 1; i < table.size(); ++i) {
        table[i] = rotateCwOnce(table[i - 1], boxSize);
    }
    return table;
}

// ---- Shapes (Spawn orientation, y up) ----
constexpr RotationTable kIRotations = makeRotations(CellArray{{{0, 2}, {1, 2}, {2, 2}, {3, 2}}}, 4);
constexpr RotationTable kORotations = makeRotations(CellArray{{{1, 1}, {2, 1}, {1, 2}, {2, 2}}}, 4);
constexpr RotationTable kTRotations = makeRotations(CellArray{{{1, 2}, {0, 1}, {1, 1}, {2, 1}}}, 3);
constexpr RotationTable kSRotations = makeRotations(CellArray{{{1, 2}, {2, 2}, {0, 1}, {1, 1}}}, 3);
constexpr RotationTable kZRotations = makeRotations(CellArray{{{0, 2}, {1, 2}, {1, 1}, {2, 1}}}, 3);
constexpr RotationTable kJRotations = makeRotations(CellArray{{{0, 2}, {0, 1}, {1, 1}, {2, 1}}}, 3);
constexpr RotationTable kLRotations = makeRotations(CellArray{{{2, 2}, {0, 1}, {1, 1}, {2, 1}}}, 3);

constexpr KickList kick(Vec2i a, Vec2i b, Vec2i c, Vec2i d, Vec2i e) noexcept {
    return KickList{{a, b, c, d, e}};
}

// ---- Kick tables. Index = from * 2 + (clockwise ? 0 : 1) ----
constexpr std::array<KickList, 8> kJlstzKicks{{
    kick({0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}),  // 0 -> R
    kick({0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}),     // 0 -> L
    kick({0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}),      // R -> 2
    kick({0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}),      // R -> 0
    kick({0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}),     // 2 -> L
    kick({0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}),  // 2 -> R
    kick({0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}),   // L -> 0
    kick({0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}),   // L -> 2
}};

constexpr std::array<KickList, 8> kIKicks{{
    kick({0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}),  // 0 -> R
    kick({0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}),  // 0 -> L
    kick({0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}),  // R -> 2
    kick({0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}),  // R -> 0
    kick({0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}),  // 2 -> L
    kick({0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}),  // 2 -> R
    kick({0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}),  // L -> 0
    kick({0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}),  // L -> 2
}};

constexpr KickList kNoKicks{};  // O: five copies of (0, 0)

constexpr std::size_t kickIndex(Rotation from, Rotation to) noexcept {
    const std::size_t direction = (to == rotatedCw(from)) ? 0U : 1U;
    return toIndex(from) * 2U + direction;
}

}  // namespace

// ---------------- IPiece ----------------
IPiece::IPiece() noexcept : Tetromino(PieceType::I, kSpawnI) {}
CellArray IPiece::cells(Rotation rotation) const noexcept { return kIRotations[toIndex(rotation)]; }
KickList IPiece::kicks(Rotation from, Rotation to) const noexcept { return kIKicks[kickIndex(from, to)]; }
std::unique_ptr<Tetromino> IPiece::clone() const { return std::make_unique<IPiece>(*this); }

// ---------------- OPiece ----------------
OPiece::OPiece() noexcept : Tetromino(PieceType::O, kSpawnDefault) {}
CellArray OPiece::cells(Rotation rotation) const noexcept { return kORotations[toIndex(rotation)]; }
KickList OPiece::kicks(Rotation /*from*/, Rotation /*to*/) const noexcept { return kNoKicks; }
std::unique_ptr<Tetromino> OPiece::clone() const { return std::make_unique<OPiece>(*this); }

// ---------------- TPiece ----------------
TPiece::TPiece() noexcept : Tetromino(PieceType::T, kSpawnDefault) {}
CellArray TPiece::cells(Rotation rotation) const noexcept { return kTRotations[toIndex(rotation)]; }
KickList TPiece::kicks(Rotation from, Rotation to) const noexcept { return kJlstzKicks[kickIndex(from, to)]; }
std::unique_ptr<Tetromino> TPiece::clone() const { return std::make_unique<TPiece>(*this); }

// ---------------- SPiece ----------------
SPiece::SPiece() noexcept : Tetromino(PieceType::S, kSpawnDefault) {}
CellArray SPiece::cells(Rotation rotation) const noexcept { return kSRotations[toIndex(rotation)]; }
KickList SPiece::kicks(Rotation from, Rotation to) const noexcept { return kJlstzKicks[kickIndex(from, to)]; }
std::unique_ptr<Tetromino> SPiece::clone() const { return std::make_unique<SPiece>(*this); }

// ---------------- ZPiece ----------------
ZPiece::ZPiece() noexcept : Tetromino(PieceType::Z, kSpawnDefault) {}
CellArray ZPiece::cells(Rotation rotation) const noexcept { return kZRotations[toIndex(rotation)]; }
KickList ZPiece::kicks(Rotation from, Rotation to) const noexcept { return kJlstzKicks[kickIndex(from, to)]; }
std::unique_ptr<Tetromino> ZPiece::clone() const { return std::make_unique<ZPiece>(*this); }

// ---------------- JPiece ----------------
JPiece::JPiece() noexcept : Tetromino(PieceType::J, kSpawnDefault) {}
CellArray JPiece::cells(Rotation rotation) const noexcept { return kJRotations[toIndex(rotation)]; }
KickList JPiece::kicks(Rotation from, Rotation to) const noexcept { return kJlstzKicks[kickIndex(from, to)]; }
std::unique_ptr<Tetromino> JPiece::clone() const { return std::make_unique<JPiece>(*this); }

// ---------------- LPiece ----------------
LPiece::LPiece() noexcept : Tetromino(PieceType::L, kSpawnDefault) {}
CellArray LPiece::cells(Rotation rotation) const noexcept { return kLRotations[toIndex(rotation)]; }
KickList LPiece::kicks(Rotation from, Rotation to) const noexcept { return kJlstzKicks[kickIndex(from, to)]; }
std::unique_ptr<Tetromino> LPiece::clone() const { return std::make_unique<LPiece>(*this); }

}  // namespace tf::core
