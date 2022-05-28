add_library(objects INTERFACE "buffers.h" "object.h" "solid_object.h" "texture_object.h")

target_include_directories(objects INTERFACE ./)