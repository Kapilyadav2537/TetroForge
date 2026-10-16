# SRS: §2.1, §2.5, §7.1 (Core is pure C++17), NFR-3
# Core links NOTHING except tf_options (compiler flags). External/GUI
# include paths are never visible here, so a stray #include fails to compile.
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
