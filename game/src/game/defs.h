#pragma once
#include "constants.h"

namespace game
{
    enum class block_type : unsigned char
    {
        block = 0,
        jump_block = 1,
        spike = 2,
    };

    enum class block_color : unsigned char
    {
        blue = 0,
        red = 1,
        neutral = 2,
    };

    enum direction : unsigned char
    {
        up = 0,
        right = 1,
        down = 2,
        left = 3,
    };

    double radians(direction dir)
    {
        return (static_cast<unsigned char>(dir) * (-math::pi() / 2));
    }

    direction opposite(direction dir)
    {
        return static_cast<direction>((static_cast<unsigned char>(dir) + 2) % 4);
    }

    direction turned_left(direction dir)
    {
        if (dir == up)
            return left;
        return static_cast<direction>(dir - 1);
    }

    direction turned_right(direction dir)
    {
        if (dir == left)
            return up;
        return static_cast<direction>(dir + 1);
    }
    
}