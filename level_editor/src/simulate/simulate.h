#pragma once
#include "timer.h"

#include "handler.h"

#include "sizes.h"

// #ifdef _WIN32
// #define main wWinMain
// #elif
// #define main main
// #endif

namespace game
{
    int simulate(application &app, const math::mat<float, 4, 4> &ortho, const std::vector<state>& current, math::dvec2 spawn, math::dvec2 end, textures& texts)
    {
        input *input_handler = app.get_inputHandler();
        callback_handler *callback_manager = app.get_callbackManager();

        bool reset_frame = false;

        auto before = callback_manager->get_framebuffer();
        callback_manager->set_framebuffer([&reset_frame](int width, int height)
                                          { glViewport(0, 0, width, height); reset_frame = true; });
        callback_manager->set_windowpos([&reset_frame](int x, int y)
                                        { reset_frame = true; });

        const key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);
        const key &a = input_handler->get_key(GLFW_KEY_A);
        const key &d = input_handler->get_key(GLFW_KEY_D);
        const key &space = input_handler->get_key(GLFW_KEY_SPACE);
        const key &f3 = input_handler->get_key(GLFW_KEY_F3);
        const key &left_shift = input_handler->get_key(GLFW_KEY_LEFT_SHIFT);
        
        const key &right_shift = input_handler->get_key(GLFW_KEY_RIGHT_SHIFT);
        const key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);

        bool show_fps = false;

        game::handler world(app, content_dims, texts);

        world.load_file(current, spawn, end);

        timer frame_timer;
        constexpr double min_dt = 1 / 60.0;

        while (!app.should_close())
        {
            frame_timer.start();

            input_handler->update();

            if (escape.is_initialPress())
            {
                callback_manager->set_framebuffer(before);
                return 0;
            }

            if (reset_frame)
            {
                reset_frame = false;
                continue;
            }

            double dt = frame_timer.seconds();

            if (dt < min_dt)
                dt = min_dt;

            world.update(dt);

            if (f3.is_initialPress())
                show_fps = !show_fps;

            if (left_click.is_initialPress())
                world.flip();

            if (space.is_initialPress())
                world.player_jump();
            
            if (left_shift.is_pressed() || right_shift.is_pressed())
                world.player_sneak();
            else
                world.stop_player_sneak();

            if (a.is_pressed())
                world.move_player_left();
            else if (d.is_pressed())
                world.move_player_right();

            glClearColor(1.f, 1.f, 1.f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT);

            world.draw(ortho);

            if (show_fps)
                graphics::print_text(app, std::to_string(static_cast<int>(1 / frame_timer.seconds())) + "fps", {10, content_dims.y - 75}, 50, {0, 0, 0}, ortho);

            app.swap_buffers();

            frame_timer.stop();
        }
        callback_manager->set_framebuffer(before);
        callback_manager->set_windowpos([](int, int){});
        return 1;
    }
}