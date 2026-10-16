#pragma once
/**
 * @file LayerInfo.hpp
 * @brief Scaffolding for the Presentation layer (removed in Week 5 with real UI code).
 * @note SRS: §7.1.
 */
#include <string>
#include <string_view>

namespace tf::ui {

/** @brief Name of the Presentation layer. */
[[nodiscard]] std::string_view layerName() noexcept;

/**
 * @brief Describes the layers above Core.
 * @return "presentation > application". Core is proven by its own tests.
 */
[[nodiscard]] std::string layerStack();

}  // namespace tf::ui
