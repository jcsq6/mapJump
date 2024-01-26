cmake_minimum_required(VERSION 3.8)

project(map_jumper)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE ASSET_FILES "src/assets/*.cpp")

add_executable(map_jumper src/src/map_jump.cpp src/src/collision.cpp src/src/game.cpp src/src/level.cpp src/src/run_game.cpp ${ASSET_FILES})
add_executable(level_editor src/src/level_editor.cpp src/src/collision.cpp src/src/game.cpp src/src/level.cpp src/src/run_game.cpp ${ASSET_FILES})

target_include_directories(map_jumper PUBLIC src/include src/assets)
target_include_directories(level_editor PUBLIC src/include src/assets dep/nativefiledialog/include)

add_compile_definitions($<$<CONFIG:Debug>:MAPJUMP_DEBUG>)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory(dep/tinyfiledialogs)

target_link_libraries(map_jumper PUBLIC OpenGL::GL GLEW::GLEW glfw Freetype::Freetype glm::glm)
target_link_libraries(level_editor PUBLIC OpenGL::GL GLEW::GLEW glfw Freetype::Freetype glm::glm tinyfiledialogs)