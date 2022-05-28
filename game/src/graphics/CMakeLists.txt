add_library(graphics INTERFACE "application.h" "input.h" "callback.h")

target_include_directories(graphics INTERFACE ./)

add_subdirectory("texture")
add_subdirectory("shaders")
add_subdirectory("objects")
add_subdirectory("text")

target_link_libraries(graphics INTERFACE texture shaders objects text)