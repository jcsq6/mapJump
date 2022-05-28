add_library(game INTERFACE "handler.h" "game_obj.h" "player.h" "loader.h" "defs.h")

target_include_directories(game INTERFACE ./)