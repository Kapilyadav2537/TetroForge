/**
 * @file LayerInfo.cpp
 * @brief Reports Application layer identifier for boundary testing.
 */
#include <tf/app/LayerInfo.hpp>

namespace tf::app {

std::string_view layerName() noexcept {
    return "application";
}

}  // namespace tf::app
