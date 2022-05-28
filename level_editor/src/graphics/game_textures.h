#pragma once
#include "texture_object.h"
#include "sizes.h"

GRAPHICS_BEG
class game_textures
{
    static void set_texture_properties(texture_object *obj, double width, double height)
    {
        static math::vec4 transparent{0.0, 0.0, 00, 0.0};
        static std::array<math::dvec2, 4> pts{{{}, {}, {}, {}}};
        // set to transparent border
        obj->set_texture_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        obj->set_texture_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        obj->set_texture_parameter(GL_TEXTURE_BORDER_COLOR, math::value(transparent));

        // set pts to correspond to correct dimensions
        pts[1][0] = pts[2][0] = width;
        pts[2][1] = pts[3][1] = height;
        obj->attach_shape(pts);

        // set points to correspond to basic texture square in text coords
        pts[1][0] = pts[2][0] = 1;
        pts[2][1] = pts[3][1] = 1;
        obj->attach_texture_shape(pts);
    }

    game_textures() : blue_cube{"assets/blue_cube.png"},
                      blue_cube_fade{"assets/blue_cube_fade.png"},
                      blue_jump{"assets/blue_jump.png"},
                      blue_spike{"assets/blue_spike.png"},
                      blue_spike_fade{"assets/blue_spike_fade.png"},
                      red_cube{"assets/red_cube.png"},
                      red_cube_fade{"assets/red_cube_fade.png"},
                      red_jump{"assets/red_jump.png"},
                      red_spike{"assets/red_spike.png"},
                      red_spike_fade{"assets/red_spike_fade.png"},
                      neutral_cube{"assets/neutral_cube.png"},
                      neutral_jump{"assets/neutral_jump.png"},
                      neutral_spike{"assets/neutral_spike.png"},
                      background{"assets/background.png"},
                      spawn_anchor{"assets/spawn_anchor.png"},
                      end_anchor{"assets/end_anchor.png"},
                      player_text{"assets/player_text.png"}
    {
        set_texture_properties(&blue_cube, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&blue_cube_fade, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&blue_jump, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&blue_spike, game::spike_size.x, game::spike_size.y);
        set_texture_properties(&blue_spike_fade, game::spike_size.x, game::spike_size.y);

        set_texture_properties(&red_cube, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&red_cube_fade, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&red_jump, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&red_spike, game::spike_size.x, game::spike_size.y);
        set_texture_properties(&red_spike_fade, game::spike_size.x, game::spike_size.y);

        set_texture_properties(&neutral_cube, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&neutral_jump, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&neutral_spike, game::spike_size.x, game::spike_size.y);

        set_texture_properties(&background, game::content_dims.x, game::content_dims.y);
        set_texture_properties(&spawn_anchor, game::cube_size.x, game::cube_size.y);
        set_texture_properties(&end_anchor, game::cube_size.x, game::cube_size.y);

        set_texture_properties(&player_text, 40, 40);
    }

public:
    static game_textures &get_instance()
    {
        static game_textures texts;
        return texts;
    }

    texture_object blue_cube;
    texture_object blue_cube_fade;
    texture_object blue_jump;
    texture_object blue_spike;
    texture_object blue_spike_fade;
    texture_object red_cube;
    texture_object red_cube_fade;
    texture_object red_jump;
    texture_object red_spike;
    texture_object red_spike_fade;
    texture_object neutral_cube;
    texture_object neutral_jump;
    texture_object neutral_spike;
    texture_object background;
    texture_object spawn_anchor;
    texture_object end_anchor;
    texture_object player_text;
};

GRAPHICS_END