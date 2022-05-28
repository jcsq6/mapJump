cmake_minimum_required(VERSION 3.4)

add_executable(map_jumper WIN32 "main.cpp")

add_subdirectory("utils")
add_subdirectory("physics")
add_subdirectory("math")
add_subdirectory("graphics")
add_subdirectory("game")


if(MSVC)
	target_compile_options(map_jumper PRIVATE $<$<CONFIG:RELEASE>:/O2 /MT> $<$<CONFIG:DEBUG>:/MTd> /W2)
else()
	target_compile_options(map_jumper PRIVATE $<$<CONFIG:DEBUG>:-g> $<$<CONFIG:RELEASE>:-O3>)
	target_link_options(map_jumper PRIVATE $<$<CONFIG:RELEASE>:-s>)
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

target_link_libraries(map_jumper PRIVATE OpenGL::GL GLEW::GLEW glfw utils physics math graphics game)