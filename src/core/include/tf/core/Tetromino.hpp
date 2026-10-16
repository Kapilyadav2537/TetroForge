#pragma once
/**
 * @file Tetromino.hpp
 * @brief Abstract base class of the seven pieces.
 * @note SRS: FR-1.2 (each piece a class derived from Tetromino), FR-1.4 (kick tables),
 *       §7.2 (Tetromino), §7.5 (Prototype via clone()), §7.6 (abstraction, runtime
 *       polymorphism, encapsulation).
 */
#include <array>
#include <memory>

#include <tf/core/Types.hpp>
#include <tf/core/Vec2i.hpp>

namespace tf::core {

/// The four cells of one orientation of a piece.
using CellArray = std::array<Vec2i, 4>;
/// The five offsets tried, in order, when a rotation is requested.
using KickList = std::array<Vec2i, 5>;

/**
 * @brief State and behaviour shared by every tetromino.
 *
 * A piece has a *position* (bottom-left corner of its bounding box in board
 * coordinates) and a *rotation*. Subclasses only supply data: the cell offsets
 * of each rotation and the kick offsets. They never touch the Board.
 *
 * Copying through a base reference is blocked (protected copy) to prevent
 * slicing; use clone() instead (Prototype pattern, used for ghost projection and hold piece).
 */
class Tetromino {
public:
    virtual ~Tetromino() = default;

    /** @brief Which of the seven pieces this is. */
    [[nodiscard]] PieceType type() const noexcept { return type_; }
    /** @brief Bottom-left corner of the bounding box in board coordinates. */
    [[nodiscard]] Vec2i position() const noexcept { return position_; }
    /** @brief Current orientation. */
    [[nodiscard]] Rotation rotation() const noexcept { return rotation_; }

    /** @brief Moves the box origin. Does no collision check; Board does that. */
    void setPosition(Vec2i position) noexcept { position_ = position; }
    /** @brief Sets the orientation. Does no collision check. */
    void setRotation(Rotation rotation) noexcept { rotation_ = rotation; }

    /**
     * @brief Cell offsets inside the bounding box for orientation @p rotation.
     * @return Four distinct offsets, each in [0, boxSize) on both axes.
     */
    [[nodiscard]] virtual CellArray cells(Rotation rotation) const noexcept = 0;

    /**
     * @brief Kick offsets for a rotation from @p from to @p to (FR-1.4).
     * @pre `to` is one quarter turn from `from`.
     * @return Five offsets added to position(); the first is always (0, 0).
     */
    [[nodiscard]] virtual KickList kicks(Rotation from, Rotation to) const noexcept = 0;

    /** @brief Polymorphic deep copy (Prototype pattern). */
    [[nodiscard]] virtual std::unique_ptr<Tetromino> clone() const = 0;

    /** @brief Board coordinates of the four cells at the current position and rotation. */
    [[nodiscard]] CellArray worldCells() const;

    /** @brief Board coordinates the cells would have at a hypothetical origin and rotation. */
    [[nodiscard]] CellArray worldCells(Vec2i origin, Rotation rotation) const;

protected:
    /** @brief Subclasses pass their type and spawn position. */
    Tetromino(PieceType pieceType, Vec2i spawn) noexcept;
    Tetromino(const Tetromino&) = default;
    Tetromino& operator=(const Tetromino&) = default;
    Tetromino(Tetromino&&) = default;
    Tetromino& operator=(Tetromino&&) = default;

private:
    PieceType type_;
    Vec2i position_;
    Rotation rotation_{Rotation::Spawn};
};

}  // namespace tf::core
