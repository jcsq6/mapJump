#include "application.h"
#include "editor.h"

int main(int argc, char *argv[])
{
    math::vec<int, 2> window_dims{1200, 675};

    auto ortho = math::ortho_mat(0, game::content_dims.x, 0, game::content_dims.y, -1, 1);

    graphics::application *app = graphics::application::get_instance("map jumper", window_dims.x, window_dims.y);
    auto *input_handler = app->get_inputHandler();
    auto *callback_manager = app->get_callbackManager();

    glfwSetWindowAspectRatio(app->get_window(), game::aspect_ratio.x, game::aspect_ratio.y);

    app->set_swapInterval(1);

    math::dvec2 mouse_pos;

    callback_manager->set_framebuffer([&window_dims](int width, int height)
                                      { window_dims = {width, height}; glViewport(0, 0, width, height); });

    callback_manager->set_cursor(
        [&](double x, double y)
        {
            mouse_pos.x = x * game::content_dims.x / window_dims.x;
            mouse_pos.y = (window_dims.y - y) * game::content_dims.y / window_dims.y;
        });

    editor::run(ortho, mouse_pos);
}