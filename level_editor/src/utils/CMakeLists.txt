add_library(utils INTERFACE "debug.h" "timer.h" "file_dialog.h")

target_include_directories(utils INTERFACE ./)