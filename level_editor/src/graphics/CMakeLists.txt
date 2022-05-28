add_library(graphics INTERFACE "game_textures.h" "button.h")

target_include_directories(graphics INTERFACE ./)

add_subdirectory("texture")
add_subdirectory("shaders")
add_subdirectory("objects")
add_subdirectory("text")
add_subdirectory("app")

target_link_libraries(graphics INTERFACE texture shaders objects text app)