# SRS: §7.1 (layered architecture)
add_subdirectory(core)
add_subdirectory(application)
add_subdirectory(presentation)

add_executable(tetroforge main.cpp)
target_link_libraries(tetroforge PRIVATE tf_presentation)
