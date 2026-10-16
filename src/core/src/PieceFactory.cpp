/** @file PieceFactory.cpp  @note SRS: §7.5 (Factory), FR-1.2. */
#include <tf/core/PieceFactory.hpp>

#include <stdexcept>

#include <tf/core/Pieces.hpp>

namespace tf::core {

std::unique_ptr<Tetromino> PieceFactory::create(PieceType type) {
    switch (type) {
        case PieceType::I: return std::make_unique<IPiece>();
        case PieceType::O: return std::make_unique<OPiece>();
        case PieceType::T: return std::make_unique<TPiece>();
        case PieceType::S: return std::make_unique<SPiece>();
        case PieceType::Z: return std::make_unique<ZPiece>();
        case PieceType::J: return std::make_unique<JPiece>();
        case PieceType::L: return std::make_unique<LPiece>();
    }
    throw std::invalid_argument("PieceFactory::create: invalid PieceType");
}

}  // namespace tf::core
