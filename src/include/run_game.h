#ifndef RUN_GAME_H
#define RUN_GAME_H

#include "gl_object.h"
#include "game.h"

constexpr int window_width = 960;
constexpr int window_height = 540;

constexpr int target_width = 960;
constexpr int target_height = 540;

int run_game(const std::filesystem::path &location);

#endif