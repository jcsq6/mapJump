add_library(math INTERFACE "constants.h" "mat.h" "vec.h")

target_include_directories(math INTERFACE ./)