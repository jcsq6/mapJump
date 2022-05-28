add_library(shaders INTERFACE "glsl_type.h" "shaders.h")

target_include_directories(shaders INTERFACE ./)