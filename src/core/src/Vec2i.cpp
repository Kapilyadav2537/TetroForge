/**
 * @file Vec2i.cpp
 * @brief Implements text streaming for 2D vectors in format (x, y).
 */
#include <tf/core/Vec2i.hpp>

#include <ostream>

namespace tf::core {

std::ostream& operator<<(std::ostream& os, Vec2i v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

}  // namespace tf::core
