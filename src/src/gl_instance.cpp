#include "gl_instance.h"

static const glm::vec2 square_pts[] = {{-.5f, -.5f}, {.5f, -.5f}, {.5f, .5f}, {-.5f, .5f}};
static const glm::vec2 triangle_pts[] = {{-.5f, -.5f}, {.5f, -.5f}, {0, .5f}};

shapes::buffers::buffers()
{
	// setup square vao
	glBindVertexArray(square_vao.id);

	// buffer square data
	glBindBuffer(GL_ARRAY_BUFFER, square_vbo.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_pts), square_pts, GL_STATIC_DRAW);

	glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(pos_attribute);

	// buffer square texture coord data
	float square_text_pos_data[4 * 2] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};

	glBindBuffer(GL_ARRAY_BUFFER, square_text_pos_vbo.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_text_pos_data), square_text_pos_data, GL_STATIC_DRAW);

	glVertexAttribPointer(text_pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(text_pos_attribute);

	// buffer triangle data
	glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_pts), triangle_pts, GL_STATIC_DRAW);

	// setup triangle vao
	glBindVertexArray(triangle_vao.id);
	glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(pos_attribute);
	
	// buffer triangle texture coord data
	float triangle_text_pos_data[3 * 2] = {
		0, 0,
		1, 0,
		.5, 1,
	};

	glBindBuffer(GL_ARRAY_BUFFER, triangle_text_pos_vbo.id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_text_pos_data), triangle_text_pos_data, GL_STATIC_DRAW);

	glVertexAttribPointer(text_pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(text_pos_attribute);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

shapes::shapes() : m_buffers{}
{
}

static constexpr const char *vertex_shader =
	"#version 330 core\n" // vertex shader
	"layout (location = 0) in vec2 pos;"
	"layout (location = 1) in vec2 tex_coord;"
	"uniform mat4 ortho;"
	"uniform mat4 model;"
	"out vec2 texture_coord;"
	"void main(){"
	"	gl_Position = ortho * model * vec4(pos, 0, 1);"
	"	texture_coord = tex_coord;"
	"}";

static constexpr const char *fragment_shader =
	"#version 330 core\n" // fragment shader
	"uniform sampler2D text;"
	"in vec2 texture_coord;"
	"out vec4 frag_color;"
	"void main(){\n"
	"	frag_color = texture(text, texture_coord);"
	"}";

gl_instance::gl_instance(int width, int height, const char *title) : m_glfw(), m_window(width, height, title), m_shapes(), m_texture_program(vertex_shader, fragment_shader), m_assets(), m_min{}, m_size{width, height}
{
	glfwSetWindowUserPointer(m_window.handle, this);
}

void gl_instance::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	int leftover_width = width % aspect_ratio_x;
	int leftover_height = height % aspect_ratio_y;

	int new_width = width - leftover_width;
	int new_height = height - leftover_height;

	int height_from_keep_x = new_width * aspect_ratio_y / aspect_ratio_x;
	int width_from_keep_y = height * aspect_ratio_x / aspect_ratio_y;

	int diff_x = new_width - width_from_keep_y;
	int diff_y = new_height - height_from_keep_x;

	// keep height
	if (diff_y < diff_x)
	{
		new_width = width_from_keep_y;
		leftover_width = width - new_width;
	}
	// keep width
	else
	{
		new_height = height_from_keep_x;
		leftover_height = height - new_height;
	}

    glViewport(leftover_width / 2, leftover_height / 2, new_width, new_height);

	gl_instance *owner = static_cast<gl_instance *>(glfwGetWindowUserPointer(window));
	if (owner->m_draw)
		owner->m_draw();
	owner->m_min.x = leftover_width / 2;
	owner->m_min.y = leftover_height / 2;
	owner->m_size.x = new_width;
	owner->m_size.y = new_height;
}

const polygon &square()
{
	static const polygon res(std::begin(square_pts), std::end(square_pts));
	return res;
}
const polygon &triangle()
{
	static const polygon res(std::begin(triangle_pts), std::end(triangle_pts));
	return res;
}