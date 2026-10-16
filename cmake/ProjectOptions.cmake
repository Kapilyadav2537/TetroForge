# Compiler flags, sanitizers, and diagnostic options configuration.
#
# One INTERFACE library carries every compiler/linker flag. Targets link it,
# so flags are defined once and applied uniformly across all targets.
add_library(tf_options INTERFACE)

if(MSVC)
  target_compile_options(tf_options INTERFACE /W4 /permissive- /utf-8)
  if(TF_WARNINGS_AS_ERRORS)
    target_compile_options(tf_options INTERFACE /WX)
  endif()
else()
  target_compile_options(tf_options INTERFACE
    -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
  if(TF_WARNINGS_AS_ERRORS)
    target_compile_options(tf_options INTERFACE -Werror)
  endif()
endif()

if(TF_ENABLE_SANITIZERS)
  if(MSVC)
    message(WARNING "TF_ENABLE_SANITIZERS is GCC/Clang only; ignored on MSVC.")
  else()
    include(CheckCXXSourceCompiles)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address,undefined")
    check_cxx_source_compiles("int main() { return 0; }" TF_SANITIZERS_SUPPORTED)
    unset(CMAKE_REQUIRED_FLAGS)
    unset(CMAKE_REQUIRED_LINK_OPTIONS)

    if(TF_SANITIZERS_SUPPORTED)
      target_compile_options(tf_options INTERFACE
        -fsanitize=address,undefined -fno-sanitize-recover=all
        -fno-omit-frame-pointer)
      target_link_options(tf_options INTERFACE -fsanitize=address,undefined)
    else()
      message(WARNING "Sanitizers requested (TF_ENABLE_SANITIZERS=ON), but linking with -fsanitize=address failed (e.g. missing libasan package on this system). Sanitizers disabled.")
    endif()
  endif()
endif()

if(TF_ENABLE_COVERAGE AND NOT MSVC)
  target_compile_options(tf_options INTERFACE --coverage -O0 -g)
  target_link_options(tf_options INTERFACE --coverage)
endif()
