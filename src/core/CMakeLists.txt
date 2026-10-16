# Core domain library (pure C++17 domain logic, zero GUI or external dependencies)
add_library(tf_core STATIC
  src/Vec2i.cpp
  src/Tetromino.cpp
  src/Pieces.cpp
  src/PieceFactory.cpp
  src/Board.cpp
  src/Game.cpp
  src/ScoreManager.cpp)
add_library(tf::core ALIAS tf_core)
target_include_directories(tf_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(tf_core PUBLIC tf_options)
