#pragma once
#include "vec.h"
#include "constants.h"

#include <array>

namespace physics
{
    struct bounding_box
    {
        bounding_box() : min{-math::infinity, -math::infinity}, max{math::infinity, math::infinity} {}
        bounding_box(double x, double y, double xlength, double ylength) : min{x, y}, max{x + xlength, y + ylength} {}
        bounding_box(math::dvec2 loc, math::dvec2 length) : min{loc}, max{loc + length} {}
        
        math::dvec2 min;
        math::dvec2 max;

        math::dvec2 dims() const
        {
            return max - min;
        }

        void update_pos(math::dvec2 new_pos)
        {
            auto d = dims();
            min = new_pos;
            max = new_pos + d;
        }

        bounding_box& operator+=(math::dvec2 offset)
        {
            min += offset;
            max += offset;
            return *this;
        }

        bounding_box& operator-=(math::dvec2 offset)
        {
            min -= offset;
            max -= offset;
            return *this;
        }

        bounding_box operator+(math::dvec2 offset)
        {
            return bounding_box(*this) += offset;
        }

        bounding_box operator-(math::dvec2 offset)
        {
            return bounding_box(*this) -= offset;
        }

        std::array<math::dvec2, 4> points() const
        {
            auto d = dims();
            return {min, min + math::dvec2{d.x, 0}, max, min + math::dvec2{0, d.y}};
        }
    };
}