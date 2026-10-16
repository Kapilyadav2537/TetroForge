#pragma once
/**
 * @file PieceFactory.hpp
 * @brief Creates pieces by type so client code needs no switch statement.
 * @note SRS: §7.2 (PieceFactory), §7.5 (Factory pattern), FR-1.2.
 */
#include <memory>

#include <tf/core/Tetromino.hpp>

namespace tf::core {

/** @brief Static factory; it holds no state, so it is not a Singleton. */
class PieceFactory {
public:
    PieceFactory() = delete;

    /**
     * @brief Builds a fresh piece at its spawn position and Spawn rotation.
     * @throws std::invalid_argument if @p type is not one of the seven values.
     */
    [[nodiscard]] static std::unique_ptr<Tetromino> create(PieceType type);
};

}  // namespace tf::core
