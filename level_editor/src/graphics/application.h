#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "input.h"
#include "callback.h"

class application
{
    GLFWwindow *window;

    static void init()
    {
        static bool is_init = false;

        if (is_init)
            return;

        is_init = true;
        glfwInit();
    }

    application(const char *name, int width, int height)
    {
        init();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        window = glfwCreateWindow(width, height, name, nullptr, nullptr);
        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK)
            throw std::runtime_error("Failed to initialize glew");
    }

public:
    template<typename... Ts>
    static void set_window_hints_true(Ts... hints)
    {
        init();
        (glfwWindowHint(hints, GLFW_TRUE), ...);
    }
    template<typename... Ts>
    static void set_window_hints_false(Ts... hints)
    {
        init();
        (glfwWindowHint(hints, GLFW_FALSE), ...);
    }
    static application *get_instance(const char *name, int width, int height)
    {
        static application instance(name, width, height);
        return &instance;
    }

    GLFWwindow *get_window()
    {
        return window;
    }

    input *get_inputHandler()
    {
        return input::get_instance(this);
    }

    callback_handler *get_callbackManager()
    {
        return callback_handler::get_instance(this);
    }

    bool should_close() const
    {
        return glfwWindowShouldClose(window);
    }

    void swap_buffers() const
    {
        glfwSwapBuffers(window);
    }

    void set_windowSize(int width, int height)
    {
        glfwSetWindowSize(window, width, height);
    }

    static void set_swapInterval(int interval)
    {
        glfwSwapInterval(interval);
    }
    
    ~application()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

void input::update()
{
    glfwPollEvents();
    for (auto &k : keys)
    {
        k.second.was_pressed = k.second.pressed;
        k.second.pressed = glfwGetKey(app->get_window(), k.first) == GLFW_PRESS;
    }
    for (auto &b : buttons)
    {
        b.second.was_pressed = b.second.pressed;
        b.second.pressed = glfwGetMouseButton(app->get_window(), b.first) == GLFW_PRESS;
    }
    glfwGetCursorPos(app->get_window(), &pos.x, &pos.y);
}

const key &input::get_key(int key_code) const
{
    if (!keys.count(key_code))
        keys[key_code].pressed = glfwGetKey(app->get_window(), key_code);
    return keys[key_code];
}

const key &input::get_mouse_button(int button_code) const
{
    if (!buttons.count(button_code))
        buttons[button_code].pressed = glfwGetMouseButton(app->get_window(), button_code);
    return buttons[button_code];
}

void callback_handler::set_windowsize(std::function<void(int, int)> callback)
{
    windowsize = std::move(callback);
    glfwSetWindowSizeCallback(app->get_window(), windowsize_callback);
}
void callback_handler::set_framebuffer(std::function<void(int, int)> callback)
{
    framebuffer = std::move(callback);
    glfwSetFramebufferSizeCallback(app->get_window(), framebuffer_callback);
}
void callback_handler::set_contentscale(std::function<void(float, float)> callback)
{
    contentscale = std::move(callback);
    glfwSetWindowContentScaleCallback(app->get_window(), contentscale_callback);
}
void callback_handler::set_windowpos(std::function<void(int, int)> callback)
{
    windowpos = std::move(callback);
    glfwSetWindowPosCallback(app->get_window(), windowpos_callback);
}
void callback_handler::set_windowminimize(std::function<void(int)> callback)
{
    windowminimize = std::move(callback);
    glfwSetWindowIconifyCallback(app->get_window(), windowminimize_callback);
}
void callback_handler::set_windowmaximize(std::function<void(int)> callback)
{
    windowmaximize = std::move(callback);
    glfwSetWindowMaximizeCallback(app->get_window(), windowmaximize_callback);
}
void callback_handler::set_windowfocus(std::function<void(int)> callback)
{
    windowfocus = std::move(callback);
    glfwSetWindowFocusCallback(app->get_window(), windowfocus_callback);
}
void callback_handler::set_key(std::function<void(int, int, int, int)> callback)
{
    key = std::move(callback);
    glfwSetKeyCallback(app->get_window(), key_callback);
}
void callback_handler::set_character(std::function<void(int)> callback)
{
    character = std::move(callback);
    glfwSetCharCallback(app->get_window(), character_callback);
}
void callback_handler::set_cursor(std::function<void(double, double)> callback)
{
    cursor = std::move(callback);
    glfwSetCursorPosCallback(app->get_window(), cursor_callback);
}
void callback_handler::set_enterexit(std::function<void(int)> callback)
{
    enter = std::move(callback);
    glfwSetCursorEnterCallback(app->get_window(), enter_callback);
}
void callback_handler::set_mousebutton(std::function<void(int, int, int)> callback)
{
    mouse_button = std::move(callback);
    glfwSetMouseButtonCallback(app->get_window(), button_callback);
}
void callback_handler::set_scroll(std::function<void(double, double)> callback)
{
    scroll = std::move(callback);
    glfwSetScrollCallback(app->get_window(), scroll_callback);
}