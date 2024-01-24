#ifndef GL_INSTANCE_H
#define GL_INSTANCE_H

#include "gl_object.h"

#include <vector>
#include <memory>
#include <optional>
#include <functional>

class gl_controlled
{
public:
    gl_controlled() = default;
    gl_controlled(const gl_controlled &) = delete;
    gl_controlled &operator=(const gl_controlled &) = delete;

    virtual bool has_data() const = 0;
    virtual void destroy_data() = 0;
    virtual ~gl_controlled() = default;
};

template <typename T>
class gl_controlled_data : public gl_controlled
{
public:
    // data is not constructed until get is called
    gl_controlled_data(std::function<T()> construct_fun) : data{std::nullopt}, constructor{std::move(construct_fun)} {}

    gl_controlled_data(const gl_controlled_data &) = delete;
    gl_controlled_data &operator=(const gl_controlled_data &) = delete;

    // checks if has data, if not, constructs
    T &get()
    {
        if (!data)
            data.emplace(constructor());
        return *data;
    }

    // checks if has data, if not, constructs
    const T &get() const
    {
        if (!data)
            data.emplace(constructor());
        return *data;
    }

    void destroy_data() override { data.reset(); }
    bool has_data() const override { return data.has_value(); }
private:
    mutable std::optional<T> data;
    std::function<T()> constructor;
};

class gl_instance
{
public:
    static gl_instance &instance()
    {
        static gl_instance i;
        return i;
    }

    gl_instance(const gl_instance &) = delete;
    gl_instance& operator=(const gl_instance &) = delete;

    ~gl_instance()
    {
        destroy_window();
        glfwTerminate();
    }

    void register_data(std::unique_ptr<gl_controlled> &&data)
    {
        m_controlled.push_back(std::move(data));
    }

    void create_window(int width, int height, const char *title)
    {
        if (m_window.handle)
            destroy_window();
        m_window = window(width, height, title);
    }

    void destroy_window()
    {
        for (auto &controlled : m_controlled)
            controlled->destroy_data();
        m_window.~window();
    }

    window &get_window()
    {
        return m_window;
    }

private:
    window m_window;
    std::vector<std::unique_ptr<gl_controlled>> m_controlled;

    gl_instance()
    {
        glfwInit();
    }
};

template <typename T, typename... Args>
T construct_fun(Args&&... args) { return T(std::forward<Args>(args)...); }

// constructs gl_controlled_data and attaches to a gl_instance
template <typename T, typename... Args>
gl_controlled_data<T>& construct_and_attach(Args&&... args)
{
    auto ptr = std::make_unique<gl_controlled_data<T>>(std::bind(construct_fun<T, Args...>, args...));
    auto &res = *ptr;
    gl_instance::instance().register_data(std::move(ptr));
    return res;
}

#endif