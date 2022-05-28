cmake_minimum_required(VERSION 3.4)

add_executable(level_editor WIN32 "main.cpp" "file_dialog.h" "saver.h" "sizes.h" "state.h" "textures.h")

add_subdirectory("utils")
add_subdirectory("physics")
add_subdirectory("math")
add_subdirectory("graphics")
add_subdirectory("game")
add_subdirectory("simulate")

target_include_directories(level_editor PUBLIC ./)

if(MSVC)
	target_compile_options(level_editor PRIVATE $<$<CONFIG:RELEASE>:/O2 /MT> $<$<CONFIG:DEBUG>:/MTd> /W2)
else()
	target_compile_options(level_editor PRIVATE $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(level_editor PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(level_editor PRIVATE OpenGL::GL GLEW::GLEW glfw utils physics math graphics game simulate)