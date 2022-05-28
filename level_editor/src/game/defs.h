#pragma once
#include "constants.h"
#include "macros.h"

GAME_BEG

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

block_color &operator++(block_color &a)
{
    unsigned char i = static_cast<unsigned char>(a);
    if (i == 2)
        i = 0;
    else
        ++i;
    a = static_cast<block_color>(i);
    return a;
}

block_color &operator--(block_color &a)
{
    unsigned char i = static_cast<unsigned char>(a);
    if (i == 0)
        i = 2;
    else
        --i;
    a = static_cast<block_color>(i);
    return a;
}

enum direction : unsigned char
{
    up = 0,
    right = 1,
    down = 2,
    left = 3,
};

block_type operator+(block_type a, int i)
{
    i %= 3;
    if (i < 0)
        i += 3;

    unsigned char ai = static_cast<unsigned char>(a);

    ai += i;

    ai %= 3;

    return static_cast<block_type>(ai);
}

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

// adjusts for rotation if neccessary
auto direction_model(math::dvec2 loc, math::dvec2 dims, direction facing_direction)
{
    math::mat<float, 4, 4> translated;
    switch (facing_direction)
    {
    case up:
        translated = math::translate<float>(loc.x, loc.y, 0);
        break;
    case right:
        translated = math::translate<float>(loc.x, loc.y + dims.y, 0);
        break;
    case down:
        translated = math::translate<float>(loc.x + dims.x, loc.y + dims.y, 0);
        break;
    case left:
        translated = math::translate<float>(loc.x + dims.x, loc.y, 0);
        break;
    }

    return translated * math::rot<float>(radians(facing_direction), 0, 0, 1);
}
GAME_END