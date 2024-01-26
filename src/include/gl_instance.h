#ifndef GL_INSTANCE_H
#define GL_INSTANCE_H

#include "gl_object.h"
#include "game_assets.h"
#include "collision.h"

static constexpr int pos_attribute = 0;
static constexpr int text_pos_attribute = 1;

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

    const window &get_window() const { return m_window; }
    const shapes &get_shapes() const { return m_shapes; }
    const shader &get_texture_program() const { return m_texture_program; }
    const game_assets &get_assets() const { return m_assets; }

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
};

const polygon &square();
const polygon &triangle();

#endif