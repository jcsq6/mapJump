add_library(physics INTERFACE "bounding_box.h" "collision.h" "particle.h")

target_include_directories(physics INTERFACE ./)