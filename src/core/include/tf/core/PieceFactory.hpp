#pragma once
/**
 * @file PieceFactory.hpp
 * @brief Factory Method pattern for instantiating concrete Tetromino pieces.
 *
 * Encapsulates object creation so external code does not require knowledge
 * of concrete piece subclasses, returning polymorphic std::unique_ptr<Tetromino>.
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
