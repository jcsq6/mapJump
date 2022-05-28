#pragma once
#include "texture_object.h"
#include "player.h"
#include "collision.h"
#include "defs.h"

namespace game
{
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

    struct game_obj
    {
        const physics::bounding_box bound;
        const math::mat<float, 4, 4> model;
        const block_type type;
        const direction facing;

        game_obj(math::dvec2 texture_loc, math::dvec2 texture_dimension,
                 math::dvec2 bound_loc, math::dvec2 bound_dimension,
                 block_type obj_type,
                 direction facing_direction = up)
        :
            bound(bound_loc, bound_dimension),
            model(direction_model(texture_loc, texture_dimension, facing_direction)),
            type(obj_type),
            facing(facing_direction)
        {
        }

        void draw(const math::mat<float, 4, 4>& projection, graphics::texture_object* image)
        {
            image->draw(model, math::identity(), projection);
        }
    };
}