add_library(simulate INTERFACE "handler.h" "game_obj.h" "player.h" "simulate.h")

target_include_directories(simulate INTERFACE ./)