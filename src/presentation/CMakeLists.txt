# Presentation UI layer (owns SFML window, screen stack, audio, and renderer)
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(SFML 2.5 COMPONENTS audio QUIET)

add_library(tf_presentation STATIC
	src/LayerInfo.cpp
	src/InputHandler.cpp
	src/SoundPlayer.cpp
	src/SFMLRenderer.cpp
	src/MenuScreen.cpp
	src/PlayScreen.cpp
	src/GameOverScreen.cpp
	src/HighScoreScreen.cpp
	src/Application.cpp)
add_library(tf::presentation ALIAS tf_presentation)
target_include_directories(tf_presentation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

if(SFML_AUDIO_FOUND OR TARGET sfml-audio)
	target_link_libraries(tf_presentation PUBLIC tf_application sfml-graphics sfml-window sfml-system sfml-audio)
	target_compile_definitions(tf_presentation PUBLIC TF_HAS_SFML_AUDIO=1)
else()
	target_link_libraries(tf_presentation PUBLIC tf_application sfml-graphics sfml-window sfml-system)
endif()
