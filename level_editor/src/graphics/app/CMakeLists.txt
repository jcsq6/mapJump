add_library(app INTERFACE "application.h" "callback.h" "input.h")

target_include_directories(app INTERFACE ./)