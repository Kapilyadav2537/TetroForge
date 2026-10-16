/** @file LayerInfo.cpp  @note SRS: §7.1. Scaffolding. */
#include <tf/ui/LayerInfo.hpp>

#include <tf/app/LayerInfo.hpp>

namespace tf::ui {

std::string_view layerName() noexcept {
    return "presentation";
}

std::string layerStack() {
    std::string result(layerName());
    result += " > ";
    result += app::layerName();
    return result;
}

}  // namespace tf::ui
