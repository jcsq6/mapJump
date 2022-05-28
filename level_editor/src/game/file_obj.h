#pragma once
#include <fstream>
#include <string>
#include <vector>
#include "vec.h"
#include "defs.h"

GAME_BEG
struct file_obj
{
    static_assert(sizeof(double) == 8, "double must be 8 bytes to be compatible with levels");

    math::dvec2 min;
    block_color col;
    block_type type;
    direction facing;

    static constexpr size_t block_size()
    {
        return sizeof(math::vec<double, 2>) + sizeof(block_color) + sizeof(block_type) + sizeof(direction);
    }
};
GAME_END