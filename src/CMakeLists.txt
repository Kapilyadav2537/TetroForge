# Layered architecture subdirectories (Core -> Application -> Presentation)
add_subdirectory(core)
add_subdirectory(application)
add_subdirectory(presentation)

# Main executable target
add_executable(tetroforge main.cpp)
target_link_libraries(tetroforge PRIVATE tf_presentation)
