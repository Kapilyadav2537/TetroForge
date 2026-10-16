#pragma once
/**
 * @file LayerInfo.hpp
 * @brief Architectural metadata for the Application layer.
 */
#include <string_view>

namespace tf::app {

/** @brief Name of the Application layer. */
[[nodiscard]] std::string_view layerName() noexcept;

}  // namespace tf::app
