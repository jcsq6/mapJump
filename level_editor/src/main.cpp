#include "application.h"
#include "editor.h"

int main(int argc, char *argv[])
{

    graphics::application *app = graphics::application::get_instance("map jumper", game::window_dims.x, game::window_dims.y);

    auto* callback_manager = app->get_callbackManager();

    glfwSetWindowAspectRatio(app->get_window(), game::aspect_ratio.x, game::aspect_ratio.y);

    app->set_swapInterval(1);

    callback_manager->set_framebuffer([&](int width, int height)
                                      { game::window_dims = {width, height}; glViewport(0, 0, width, height); });

    editor::run();
}