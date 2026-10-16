/**
 * @file test_layers.cpp
 * @brief Layer wiring tests: Application and Presentation link, and Core is reachable.
 * @note SRS: §7.1, NFR-6, NFR-7.
 */
#include <catch2/catch_test_macros.hpp>

#include <tf/app/LayerInfo.hpp>
#include <tf/core/Types.hpp>
#include <tf/ui/LayerInfo.hpp>

TEST_CASE("application and presentation report their names", "[layers]") {
    REQUIRE(tf::app::layerName() == "application");
    REQUIRE(tf::ui::layerName() == "presentation");
}

TEST_CASE("presentation chains to application", "[layers]") {
    REQUIRE(tf::ui::layerStack() == "presentation > application");
}

TEST_CASE("core is reachable through the layer chain", "[layers]") {
    STATIC_REQUIRE(tf::core::kBoardWidth == 10);
}
