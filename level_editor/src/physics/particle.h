#pragma once
#include "vec.h"

PHYSICS_BEG
struct particle
{
    math::vec<double, 2> vel;
    math::vec<double, 2> acc;
    math::vec<double, 2> pos;

    void update(double seconds, double xfriction)
    {
        pos += vel * seconds;
        vel += acc * seconds;
        vel.x *= (1 - xfriction);
    }
};
PHYSICS_END