# SRS: NFR-6 (portability), NFR-7 (maintainability), §2.5 (C++17, CMake build)
cmake_minimum_required(VERSION 3.20)
project(TetroForge VERSION 0.1.0
        DESCRIPTION "Tetris-style puzzle game with a custom UI toolkit"
        LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed. Use: cmake -S . -B build")
endif()

# Clean up runtime highscores.txt from source directory if present
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/highscores.txt")
  file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/highscores.txt")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)            # -std=c++17, not gnu++17 (portability)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)    # needed by clang-tidy

option(TF_BUILD_TESTS        "Build the Catch2 test suite"       ON)
option(TF_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(TF_ENABLE_SANITIZERS  "Enable ASan + UBSan (GCC/Clang)"   OFF)
option(TF_ENABLE_COVERAGE    "Enable gcov instrumentation"       OFF)
include(cmake/ProjectOptions.cmake)

add_subdirectory(src)

if(TF_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
  enable_testing()                       # must be called in the root directory
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake")
    include(cmake/Dependencies.cmake)
  endif()
  add_subdirectory(tests)
endif()
