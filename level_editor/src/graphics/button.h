#pragma once
#include "text.h"
#include "collision.h"
#include "solid_object.h"
#include "macros.h"

GRAPHICS_BEG
struct button
{
    static constexpr math::vec3 dormant_col{33.f / 255, 44.f / 255, 94.f / 255};
    static constexpr math::vec3 hover_col{63.f / 255, 81.f / 255, 181.f / 255};

    solid_object textre;
    physics::bounding_box bound;
    math::dvec2 text_loc;
    std::string_view txt;
    bool hovered;

    button(math::dvec2 center, math::dvec2 dims, std::string_view text, math::vec4 back_color) : textre{},
                                                                                                 bound{center - dims / 2, dims},
                                                                                                 text_loc{center.x - graphics::text_width(text, .75 * dims.y) / 2, bound.min.y + .25 * dims.y},
                                                                                                 txt{text}, hovered{false}
    {
        textre.attach_shape(bound.points());
        textre.set_color(back_color);
    }

    void draw(const math::mat<float, 4, 4> &ortho)
    {
        textre.draw(math::identity(), math::identity(), ortho);
        print_text(txt, text_loc, .75 * bound.dims().y, hovered ? hover_col : dormant_col, ortho);
    }

    void get_hovered(math::dvec2 mouse_pos)
    {
        hovered = physics::is_inside(mouse_pos, bound);
    }
};
GRAPHICS_END