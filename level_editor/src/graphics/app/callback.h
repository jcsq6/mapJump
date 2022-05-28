#pragma once
#include <functional>
#include <GLFW/glfw3.h>
#include "macros.h"

GRAPHICS_BEG

class callback_handler
{
    std::function<void(int, int)> windowsize;
    std::function<void(int, int)> framebuffer;
    std::function<void(float, float)> contentscale;
    std::function<void(int, int)> windowpos;
    std::function<void(int)> windowminimize;
    std::function<void(int)> windowmaximize;
    std::function<void(int)> windowfocus;
    std::function<void(int, int, int, int)> key;
    std::function<void(unsigned int)> character;
    std::function<void(double, double)> cursor;
    std::function<void(int)> enter;
    std::function<void(int, int, int)> mouse_button;
    std::function<void(double, double)> scroll;

    callback_handler() = default;
public:
    static callback_handler *get_instance()
    {
        static callback_handler instance;
        return &instance;
    }

    void set_windowsize(std::function<void(int, int)> callback);
    void set_framebuffer(std::function<void(int, int)> callback);
    void set_contentscale(std::function<void(float, float)> callback);
    void set_windowpos(std::function<void(int, int)> callback);
    void set_windowminimize(std::function<void(int)> callback);
    void set_windowmaximize(std::function<void(int)> callback);
    void set_windowfocus(std::function<void(int)> callback);
    void set_key(std::function<void(int, int, int, int)> callback);
    void set_character(std::function<void(int)> callback);
    void set_cursor(std::function<void(double, double)> callback);
    void set_enterexit(std::function<void(int)> callback);
    void set_mousebutton(std::function<void(int, int, int)> callback);
    void set_scroll(std::function<void(double, double)> callback);

    auto get_framebuffer() const
    {
        return framebuffer;
    }
    auto get_windowfocus() const
    {
        return windowfocus;
    }
private:
    static void windowsize_callback(GLFWwindow* window, int width, int height)
    {
        get_instance()->windowsize(width, height);
    }
    static void framebuffer_callback(GLFWwindow* window, int width, int height)
    {
        get_instance()->framebuffer(width, height);
    }
    static void contentscale_callback(GLFWwindow* window, float xscale, float yscale)
    {
        get_instance()->contentscale(xscale, yscale);
    }
    static void windowpos_callback(GLFWwindow* window, int x, int y)
    {
        get_instance()->windowpos(x, y);
    }
    static void windowminimize_callback(GLFWwindow* window, int minimized)
    {
        get_instance()->windowminimize(minimized);
    }
    static void windowmaximize_callback(GLFWwindow* window, int maximized)
    {
        get_instance()->windowmaximize(maximized);
    }
    static void windowfocus_callback(GLFWwindow* window, int focused)
    {
        get_instance()->windowfocus(focused);
    }
    static void key_callback(GLFWwindow *window, int _key, int scancode, int action, int mods)
    {
        get_instance()->key(_key, scancode, action, mods);
    }
    static void character_callback(GLFWwindow* window, unsigned int codepoint)
    {
        get_instance()->character(codepoint);
    }
    static void cursor_callback(GLFWwindow* window, double x, double y)
    {
        get_instance()->cursor(x, y);
    }
    static void enter_callback(GLFWwindow* window, int entered)
    {
        get_instance()->enter(entered);
    }
    static void button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        get_instance()->mouse_button(button, action, mods);
    }
    static void scroll_callback(GLFWwindow* window, double x, double y)
    {
        get_instance()->scroll(x, y);
    }
};

GRAPHICS_END