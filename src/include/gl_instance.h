#ifndef GL_INSTANCE_H
#define GL_INSTANCE_H

#include "gl_object.h"
#include "game_assets.h"
#include "collision.h"

#include <functional>

static constexpr int pos_attribute = 0;
static constexpr int text_pos_attribute = 1;

constexpr int target_width = 960;
constexpr int target_height = 540;

constexpr int aspect_ratio_x = 16;
constexpr int aspect_ratio_y = 9;

struct shapes
{
	const vao &square_vao() const
    {
        return m_buffers.square_vao;
    }
	const vao &triangle_vao() const
    {
        return m_buffers.triangle_vao;
    }

private:
    friend class gl_instance;

	shapes();

    struct buffers
    {
        buffers();

        vao square_vao;
        vao triangle_vao;

        vbo square_vbo;
        vbo triangle_vbo;
        vbo triangle_text_pos_vbo;
        vbo square_text_pos_vbo;
    };

    buffers m_buffers;
};

class gl_instance
{
public:
    gl_instance(int width, int height, const char *title);

    gl_instance(const gl_instance &) = delete;
    gl_instance& operator=(const gl_instance &) = delete;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    const window &get_window() const { return m_window; }
    const shapes &get_shapes() const { return m_shapes; }
    const shader &get_texture_program() const { return m_texture_program; }
    const game_assets &get_assets() const { return m_assets; }

    glm::ivec2 viewport_min() const { return m_min; }
    glm::ivec2 viewport_size() const { return m_size; }

    // for continuous drawing while resizing window
    void register_draw_function(std::function<void()> draw) { m_draw = std::move(draw); }

private:
    struct glfw_instance
    {
        glfw_instance() { glfwInit(); }
        ~glfw_instance() { glfwTerminate(); }
    };

    glfw_instance m_glfw;
    window m_window;
    shapes m_shapes;
    shader m_texture_program;
    game_assets m_assets;
    std::function<void()> m_draw;
    glm::ivec2 m_min;
    glm::ivec2 m_size;
};

const polygon &square();
const polygon &triangle();

#endif