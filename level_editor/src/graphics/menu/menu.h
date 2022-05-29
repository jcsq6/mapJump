#pragma once
#include "button.h"
#include "sizes.h"
#include <list>
#include <utility>

GRAPHICS_BEG
class menu
{
public:
    menu(double button) : sz{button} {}

    struct tag
    {
    };

    static inline tag escape_clicked;
    static inline tag window_closed;

    [[nodiscard]] tag* add_button(std::string_view text)
    {
        buttons.push_front(std::make_pair<tag, button>(
            tag{}, 
            button{game::content_dims / 2, {text_width(text, sz) + 20, sz + 20}, text, {0.0, 0.0, 0.0, .2}}
        ));

        double height = sz;
        double padding = 80;

        double total = (height + padding) * buttons.size() - padding;

        double y = (game::content_dims.y - total) / 2;
        
        for (auto& b : buttons)
        {
            b.second.update_pos({b.second.bound.min.x, y});
            y += height + padding;
        }

        return &buttons.front().first;
    }

    tag *run()
    {
        static graphics::application &app = *graphics::application::get_instance();

        static auto *input_handler = app.get_inputHandler();

        static const graphics::key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);
        static const graphics::key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);

        while (!app.should_close())
        {
            input_handler->update_wait();

            if (escape.is_initialPress())
                return &escape_clicked;

            glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            auto mouse_pos = input_handler->mouse_pos();

            mouse_pos.x = mouse_pos.x * game::content_dims.x / game::window_dims.x;
            mouse_pos.y = (game::window_dims.y - mouse_pos.y) * game::content_dims.y / game::window_dims.y;

            for (auto& b : buttons)
            {
                b.second.get_hovered(mouse_pos);
                b.second.draw();
                if (b.second.hovered && left_click.is_initialPress())
                    return &b.first;
            }

            app.swap_buffers();
        }
        return &window_closed;
    }

private:
    std::list<std::pair<tag, button>> buttons;
    double sz;
};
GRAPHICS_END