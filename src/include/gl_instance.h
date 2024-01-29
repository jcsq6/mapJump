#ifndef GL_INSTANCE_H
#define GL_INSTANCE_H

#include "gl_object.h"
#include "game_assets.h"
#include "collision.h"
#include "text.h"

#include "utility.h"

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
    const shader &get_text_program() const { return m_text_program; }
    const shader &get_shape_program() const { return m_shape_program; }
    const game_assets &get_assets() const { return m_assets; }
    const glm::mat4 &get_ortho() const { return m_ortho; }
    font &get_font() { return m_font; }

    key &get_escape_key() { return m_escape; }
    key &get_left_click() { return m_left_click; }

    // adjusted for dpi
    glm::ivec2 viewport_min() const { return m_min; }
    // adjusted for dpi
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
    shader m_text_program;
    shader m_shape_program;
    game_assets m_assets;
    glm::mat4 m_ortho;
    font m_font;
    std::function<void()> m_draw;
    glm::ivec2 m_min;
    glm::ivec2 m_size;

    key m_left_click;
    key m_escape;
};

glm::dvec2 get_mouse_pos(gl_instance &gl);

const polygon &square();
const polygon &triangle();

#endif