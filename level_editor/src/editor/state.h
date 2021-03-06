#pragma once
#include "game_textures.h"
#include "file_obj.h"
#include "mat.h"
#include "macros.h"

EDITOR_BEG

void force_grid(math::dvec2& pos)
{
    pos.x = std::floor(pos.x / game::cube_size.x) * game::cube_size.x;
    pos.y = std::floor(pos.y / game::cube_size.y) * game::cube_size.y;

    if (pos.x < 0)
        pos.x = 0;
    else if (pos.x > game::usable_area.x - game::cube_size.x)
        pos.x = game::usable_area.x - game::cube_size.x;
    if (pos.y < 0)
        pos.y = 0;
    else if (pos.y > game::usable_area.y - game::cube_size.y)
        pos.y = game::usable_area.y - game::cube_size.y;
}

struct state
{
    game::file_obj file_obj;
    graphics::texture_object *texture_obj;
    math::dvec2 size;

    state() = default;
    state(game::block_color col, game::block_type type) : file_obj{{}, col, type, game::up}
    {
        update({});
    }

    void draw(const math::mat<float, 4, 4>& ortho)
    {
        texture_obj->draw(game::direction_model(file_obj.min, size, file_obj.facing), math::identity(), ortho);
    }

    void update_texture()
    {
        static graphics::game_textures& all = graphics::game_textures::get_instance();
        switch (file_obj.col)
        {
        case game::block_color::blue:
            switch (file_obj.type)
            {
            case game::block_type::block:
                texture_obj = &all.blue_cube;
                break;
            case game::block_type::jump_block:
                texture_obj = &all.blue_jump;
                break;
            case game::block_type::spike:
                texture_obj = &all.blue_spike;
            }
            break;
        case game::block_color::red:
            switch (file_obj.type)
            {
            case game::block_type::block:
                texture_obj = &all.red_cube;
                break;
            case game::block_type::jump_block:
                texture_obj = &all.red_jump;
                break;
            case game::block_type::spike:
                texture_obj = &all.red_spike;
            }
            break;
        case game::block_color::neutral:
            switch (file_obj.type)
            {
            case game::block_type::block:
                texture_obj = &all.neutral_cube;
                break;
            case game::block_type::jump_block:
                texture_obj = &all.neutral_jump;
                break;
            case game::block_type::spike:
                texture_obj = &all.neutral_spike;
            }
            break;
        }
    }

    void update(math::dvec2 pos)
    {
        force_grid(pos);

        switch (file_obj.type)
        {
        case game::block_type::block:
        case game::block_type::jump_block:
            size = game::cube_size;
            break;
        case game::block_type::spike:
            if (file_obj.facing == game::right || file_obj.facing == game::left)
                size = {game::spike_size.y, game::spike_size.x};
            else
                size = game::spike_size;
        }
        if (file_obj.facing == game::left || file_obj.facing == game::down)
            file_obj.min = pos + game::cube_size - size;
        else
            file_obj.min = pos;

        update_texture();
    }
};

EDITOR_END