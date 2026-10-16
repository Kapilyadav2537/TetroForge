# Architectural layering validation: ensures Core remains pure C++17 without GUI,
# third-party headers, or non-deterministic platform clocks/randomness.
#
# Usage: cmake -DCORE_DIR=<path to src/core> -P CheckLayering.cmake
# Exits non-zero (so CTest fails) if any Core file matches a forbidden pattern.
cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED CORE_DIR)
  message(FATAL_ERROR "Usage: cmake -DCORE_DIR=<dir> -P CheckLayering.cmake")
endif()

set(forbidden_patterns
  "#[ \t]*include[ \t]*[<\"](SFML|sqlite3|nlohmann|imgui)"
  "std::chrono::(system|steady|high_resolution)_clock"
  "std::(random_device|shuffle|uniform_int_distribution)")

file(GLOB_RECURSE core_files "${CORE_DIR}/*.hpp" "${CORE_DIR}/*.cpp")

set(violations "")
foreach(path IN LISTS core_files)
  file(READ "${path}" text)
  foreach(pattern IN LISTS forbidden_patterns)
    if(text MATCHES "${pattern}")
      list(APPEND violations "${path}  matches  ${pattern}")
    endif()
  endforeach()
endforeach()

list(LENGTH core_files file_count)
list(LENGTH violations violation_count)
if(violation_count GREATER 0)
  string(REPLACE ";" "\n  " report "${violations}")
  message(FATAL_ERROR "Layering violation(s) in Core:\n  ${report}")
endif()
message(STATUS "Core layering OK (${file_count} files scanned)")
