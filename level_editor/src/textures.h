#pragma once
#include "texture_object.h"
#include "sizes.h"

class textures
{
    static void set_texture_properties(graphics::texture_object *obj, double width, double height)
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

public:
    textures(application &app) : blue_cube{app, "assets/blue_cube.png"},
                                 blue_cube_fade{app, "assets/blue_cube_fade.png"},
                                 blue_jump{app, "assets/blue_jump.png"},
                                 blue_spike{app, "assets/blue_spike.png"},
                                 blue_spike_fade{app, "assets/blue_spike_fade.png"},
                                 red_cube{app, "assets/red_cube.png"},
                                 red_cube_fade{app, "assets/red_cube_fade.png"},
                                 red_jump{app, "assets/red_jump.png"},
                                 red_spike{app, "assets/red_spike.png"},
                                 red_spike_fade{app, "assets/red_spike_fade.png"},
                                 neutral_cube{app, "assets/neutral_cube.png"},
                                 neutral_jump{app, "assets/neutral_jump.png"},
                                 neutral_spike{app, "assets/neutral_spike.png"},
                                 background{app, "assets/background.png"},
                                 spawn_anchor{app, "assets/spawn_anchor.png"},
                                 end_anchor{app, "assets/end_anchor.png"},
                                 player_text{app, "assets/player_text.png"}
    {
        set_texture_properties(&blue_cube, cube_size.x, cube_size.y);
        set_texture_properties(&blue_cube_fade, cube_size.x, cube_size.y);
        set_texture_properties(&blue_jump, cube_size.x, cube_size.y);
        set_texture_properties(&blue_spike, spike_size.x, spike_size.y);
        set_texture_properties(&blue_spike_fade, spike_size.x, spike_size.y);

        set_texture_properties(&red_cube, cube_size.x, cube_size.y);
        set_texture_properties(&red_cube_fade, cube_size.x, cube_size.y);
        set_texture_properties(&red_jump, cube_size.x, cube_size.y);
        set_texture_properties(&red_spike, spike_size.x, spike_size.y);
        set_texture_properties(&red_spike_fade, spike_size.x, spike_size.y);

        set_texture_properties(&neutral_cube, cube_size.x, cube_size.y);
        set_texture_properties(&neutral_jump, cube_size.x, cube_size.y);
        set_texture_properties(&neutral_spike, spike_size.x, spike_size.y);

        set_texture_properties(&background, content_dims.x, content_dims.y);
        set_texture_properties(&spawn_anchor, cube_size.x, cube_size.y);
        set_texture_properties(&end_anchor, cube_size.x, cube_size.y);

        set_texture_properties(&player_text, 40, 40);
    }

    graphics::texture_object blue_cube;
    graphics::texture_object blue_cube_fade;
    graphics::texture_object blue_jump;
    graphics::texture_object blue_spike;
    graphics::texture_object blue_spike_fade;
    graphics::texture_object red_cube;
    graphics::texture_object red_cube_fade;
    graphics::texture_object red_jump;
    graphics::texture_object red_spike;
    graphics::texture_object red_spike_fade;
    graphics::texture_object neutral_cube;
    graphics::texture_object neutral_jump;
    graphics::texture_object neutral_spike;
    graphics::texture_object background;
    graphics::texture_object spawn_anchor;
    graphics::texture_object end_anchor;
    graphics::texture_object player_text;
};

void draw_grid(application& app, const math::mat<float, 4, 4>& ortho)
{
    struct o
    {
        graphics::solid_object obj;
        o(application& app, math::dvec2 cube_size) : obj{app}
        {
            physics::bounding_box bound({}, cube_size);
            obj.attach_shape(bound.points());
            obj.set_color({0, 0, 0, .1});
        }
    };
    static o text(app, cube_size);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (double x = 0; x < content_dims.x; x += cube_size.x)
    {
        for (double y = 0; y < content_dims.y; y += cube_size.y)
        {
            text.obj.draw(math::translate<float>(x, y, 0), math::identity(), ortho);
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}