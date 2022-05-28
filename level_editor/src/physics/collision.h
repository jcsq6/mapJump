#pragma once
#include "debug.h"
#include "bounding_box.h"

namespace physics
{
    bool collides_non_inclusive(bounding_box a, bounding_box b)
    {
        return a.min.x < b.max.x && a.max.x > b.min.x &&
               a.min.y < b.max.y && a.max.y > b.min.y;
    }
    bool collides(bounding_box a, bounding_box b)
    {
        return a.min.x <= b.max.x && a.max.x >= b.min.x &&
               a.min.y <= b.max.y && a.max.y >= b.min.y;
    }
    bool is_inside(math::dvec2 pt, bounding_box a)
    {
        return pt.x >= a.min.x && pt.x <= a.max.x &&
               pt.y >= a.min.y && pt.y <= a.max.y;
    }
    math::dvec2 mtv(bounding_box a, bounding_box b)
    {
        math::dvec2 diff;
        if (a.max.x < b.max.x)
            diff.x = a.max.x - b.min.x;
        else
            diff.x = a.min.x - b.max.x;

        if (a.max.y < b.max.y)
            diff.y = a.max.y - b.min.y;
        else
            diff.y = a.min.y - b.max.y;

        if (std::abs(diff.x) < std::abs(diff.y))
            return {diff.x, 0};
        return {0, diff.y};
    }
}