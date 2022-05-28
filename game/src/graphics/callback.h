#pragma once
#include <functional>
#include <GLFW/glfw3.h>

class application;

class callback_handler
{
    application* app;
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

    callback_handler(application* a) : app{a} {}
public:
    static callback_handler *get_instance(application *app)
    {
        static callback_handler instance(app);
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

private:
    static void windowsize_callback(GLFWwindow* window, int width, int height)
    {
        get_instance(nullptr)->windowsize(width, height);
    }
    static void framebuffer_callback(GLFWwindow* window, int width, int height)
    {
        get_instance(nullptr)->framebuffer(width, height);
    }
    static void contentscale_callback(GLFWwindow* window, float xscale, float yscale)
    {
        get_instance(nullptr)->contentscale(xscale, yscale);
    }
    static void windowpos_callback(GLFWwindow* window, int x, int y)
    {
        get_instance(nullptr)->windowpos(x, y);
    }
    static void windowminimize_callback(GLFWwindow* window, int minimized)
    {
        get_instance(nullptr)->windowminimize(minimized);
    }
    static void windowmaximize_callback(GLFWwindow* window, int maximized)
    {
        get_instance(nullptr)->windowmaximize(maximized);
    }
    static void windowfocus_callback(GLFWwindow* window, int focused)
    {
        get_instance(nullptr)->windowfocus(focused);
    }
    static void key_callback(GLFWwindow *window, int _key, int scancode, int action, int mods)
    {
        get_instance(nullptr)->key(_key, scancode, action, mods);
    }
    static void character_callback(GLFWwindow* window, unsigned int codepoint)
    {
        get_instance(nullptr)->character(codepoint);
    }
    static void cursor_callback(GLFWwindow* window, double x, double y)
    {
        get_instance(nullptr)->cursor(x, y);
    }
    static void enter_callback(GLFWwindow* window, int entered)
    {
        get_instance(nullptr)->enter(entered);
    }
    static void button_callback(GLFWwindow *window, int button, int action, int mods)
    {
        get_instance(nullptr)->mouse_button(button, action, mods);
    }
    static void scroll_callback(GLFWwindow* window, double x, double y)
    {
        get_instance(nullptr)->scroll(x, y);
    }
};