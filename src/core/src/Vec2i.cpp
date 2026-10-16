/** @file Vec2i.cpp  @note SRS: §7.6 (operator overloading). */
#include <tf/core/Vec2i.hpp>

#include <ostream>

namespace tf::core {

std::ostream& operator<<(std::ostream& os, Vec2i v) {
    return os << '(' << v.x << ", " << v.y << ')';
}

}  // namespace tf::core
