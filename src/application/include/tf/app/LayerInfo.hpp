#pragma once
/**
 * @file LayerInfo.hpp
 * @brief Week-1 scaffolding for the Application layer.
 * @note SRS: §7.1. Deleted in Week 2.
 */
#include <string_view>

namespace tf::app {

/** @brief Name of the Application layer. */
[[nodiscard]] std::string_view layerName() noexcept;

}  // namespace tf::app
