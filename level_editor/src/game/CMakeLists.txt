add_library(game INTERFACE "file_obj.h" "defs.h" "game_obj.h" "handler.h" "player.h" "simulate.h" "sizes.h")

target_include_directories(game INTERFACE ./)