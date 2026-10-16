#pragma once
/**
 * @file LayerInfo.hpp
 * @brief Architectural metadata for the Presentation layer.
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
