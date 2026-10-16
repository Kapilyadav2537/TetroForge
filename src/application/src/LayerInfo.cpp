/** @file LayerInfo.cpp  @note SRS: §7.1. Week-1 scaffolding. */
#include <tf/app/LayerInfo.hpp>

namespace tf::app {

std::string_view layerName() noexcept {
    return "application";
}

}  // namespace tf::app
