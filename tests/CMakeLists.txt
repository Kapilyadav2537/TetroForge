# Catch2 test suite and architectural layering check
add_executable(tf_tests
  test_layers.cpp
  test_vec2i.cpp
  test_grid.cpp
  test_pieces.cpp
  test_kicks.cpp
  test_board.cpp
  test_game.cpp
  test_scores.cpp)
target_include_directories(tf_tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(tf_tests PRIVATE tf_presentation Catch2::Catch2WithMain)
catch_discover_tests(tf_tests)   # each TEST_CASE becomes its own CTest entry

add_test(NAME layering.core_has_no_forbidden_includes
         COMMAND ${CMAKE_COMMAND}
                 -DCORE_DIR=${PROJECT_SOURCE_DIR}/src/core
                 -P ${PROJECT_SOURCE_DIR}/cmake/CheckLayering.cmake)
