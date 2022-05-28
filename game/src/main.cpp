#include "application.h"
#include "shaders.h"

#include "timer.h"

#include "solid_object.h"
#include "texture_object.h"

#include "bounding_box.h"

#include "object.h"

#include "handler.h"

#include "text.h"

// #ifdef _WIN32
// #define main wWinMain
// #elif
// #define main main
// #endif

int main(int argc, char *argv[])
{
    constexpr math::vec<int, 2> content_dims{1920, 1080};
    constexpr math::vec<int, 2> initial_window_dims{1200, 675};
    // must use [] operator on vec to maintain constexpr (active union member)
    constexpr math::vec<int, 2> aspect_ratio{content_dims / math::gcd(content_dims[0], content_dims[1])};

    auto ortho = math::ortho_mat(0, content_dims.x, 0, content_dims.y, -1, 1);
    
    application *app = application::get_instance("map jumper", initial_window_dims.x, initial_window_dims.y);
    input *input_handler = app->get_inputHandler();
    callback_handler* callback_manager = app->get_callbackManager();

    glfwSetWindowAspectRatio(app->get_window(), aspect_ratio.x, aspect_ratio.y);

    bool reset_frame = false;

    callback_manager->set_framebuffer([&reset_frame](int width, int height){ glViewport(0, 0, width, height); reset_frame = true; });
    callback_manager->set_windowpos([&reset_frame](int x, int y){ reset_frame = true; });

    app->set_swapInterval(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);
    const key &a = input_handler->get_key(GLFW_KEY_A);
    const key &d = input_handler->get_key(GLFW_KEY_D);
    const key &space = input_handler->get_key(GLFW_KEY_SPACE);
    const key &f3 = input_handler->get_key(GLFW_KEY_F3);
    const key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);

    bool show_fps = false;

    game::handler world(*app, content_dims);

    world.load_file("levels/level1.lvl");
    
    timer frame_timer;
    constexpr double min_dt = 1 / 60.0;

    while (!app->should_close())
    {        
        frame_timer.start();

        input_handler->update();

        if (escape.is_initialPress())
            glfwSetWindowShouldClose(app->get_window(), true);
        
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
        
        if (a.is_pressed())
            world.move_player_left();
        else if (d.is_pressed())
            world.move_player_right();

        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        world.draw(ortho);

        // graphics::print_text(*app,
        //     std::to_string(world.player_velocity().x),
        //     content_dims - math::vec<int, 2>{800, 100}, 40, {0, 0, 0}, ortho);

        if (show_fps)
            graphics::print_text(*app, std::to_string(static_cast<int>(1 / frame_timer.seconds())) + "fps", {10, content_dims.y - 75}, 50, {0, 0, 0}, ortho);

        app->swap_buffers();

        frame_timer.stop();
    }
}