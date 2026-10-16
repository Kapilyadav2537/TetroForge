#pragma once
/**
 * @file Pieces.hpp
 * @brief Concrete implementations of the seven standard tetromino shapes.
 *
 * Implements I, O, T, S, Z, J, and L pieces as subclasses of Tetromino.
 * Each class provides its specific 4-state rotation cell offsets and SRS kick offsets.
 */
#include <memory>

#include <tf/core/Tetromino.hpp>

namespace tf::core {

/** @brief The I piece (4x4 box, own kick table). */
class IPiece final : public Tetromino {
public:
    IPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

/** @brief The O piece (rotation never changes its cells; no kicks). */
class OPiece final : public Tetromino {
public:
    OPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

/** @brief The T piece (3x3 box, JLSTZ kicks). */
class TPiece final : public Tetromino {
public:
    TPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

/** @brief The S piece (3x3 box, JLSTZ kicks). */
class SPiece final : public Tetromino {
public:
    SPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

/** @brief The Z piece (3x3 box, JLSTZ kicks). */
class ZPiece final : public Tetromino {
public:
    ZPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

/** @brief The J piece (3x3 box, JLSTZ kicks). */
class JPiece final : public Tetromino {
public:
    JPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

/** @brief The L piece (3x3 box, JLSTZ kicks). */
class LPiece final : public Tetromino {
public:
    LPiece() noexcept;
    [[nodiscard]] CellArray cells(Rotation rotation) const noexcept override;
    [[nodiscard]] KickList kicks(Rotation from, Rotation to) const noexcept override;
    [[nodiscard]] std::unique_ptr<Tetromino> clone() const override;
};

}  // namespace tf::core
