cmake_minimum_required(VERSION 3.8)

project(map_jumper)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE ASSET_FILES "src/assets/*.cpp")

add_executable(map_jumper src/main.cpp src/collision.cpp src/game.cpp ${ASSET_FILES})

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)

target_link_libraries(map_jumper PUBLIC OpenGL::GL GLEW::GLEW glfw Freetype::Freetype glm::glm)