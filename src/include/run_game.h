#ifndef RUN_GAME_H
#define RUN_GAME_H

#include "gl_object.h"
#include "game.h"

constexpr int target_width = 960;
constexpr int target_height = 540;

constexpr int aspect_ratio_x = 16;
constexpr int aspect_ratio_y = 9;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int run_game(std::vector<level> &&levels);

#endif