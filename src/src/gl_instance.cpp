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

static constexpr const char *texture_vert =
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

static constexpr const char *texture_frag =
	"#version 330 core\n" // fragment shader
	"uniform sampler2D text;"
	"in vec2 texture_coord;"
	"out vec4 frag_color;"
	"void main(){\n"
	"	frag_color = texture(text, texture_coord);"
	"}";


static constexpr const char *text_vert =
	"#version 330 core\n" // vertex shader
	"layout (location = 0) in vec2 pos;"
	"layout (location = 1) in vec2 tex_coord;"
	"uniform mat4 ortho;"
	"uniform mat4 model;"
	"out vec2 texture_coord;"
	"void main() {"
	"	gl_Position = ortho * model * vec4(pos, 0.0, 1.0);"
	"	texture_coord = tex_coord;"
	"}";
static constexpr const char *text_frag =
	"#version 330 core\n" // fragment shader
	"uniform sampler2D text;"
	"uniform vec4 color;"
	"in vec2 texture_coord;"
	"out vec4 frag_color;"
	"void main() {"
	"	frag_color = vec4(color.xyz, color.w * texture(text, texture_coord).r);"
	"}";


static constexpr const char *shape_vert =
	"#version 330 core\n" // vertex shader
	"layout (location = 0) in vec2 pos;"
	"uniform mat4 ortho;"
	"uniform mat4 model;"
	"void main(){"
	"	gl_Position = ortho * model * vec4(pos, 0, 1);"
	"}";

static constexpr const char *shape_frag =
	"#version 330 core\n" // fragment shader
	"uniform vec4 color;"
	"out vec4 frag_color;"
	"void main(){\n"
	"	frag_color = color;"
	"}";

gl_instance::gl_instance(int width, int height, const char *title) :
	m_glfw(), m_window(width, height, title), m_shapes(),
	m_texture_program(texture_vert, texture_frag), m_text_program(text_vert, text_frag), m_shape_program(shape_vert, shape_frag),
	m_assets(),
	m_ortho(glm::ortho<float>(0, (float)target_width, 0, (float)target_height, -1, 1)),
	m_font(arial_data, sizeof(arial_data), 256),
	m_min{}, m_size{width, height}
{
	glfwSetWindowUserPointer(m_window.handle, this);
	glfwSetFramebufferSizeCallback(m_window.handle, framebuffer_size_callback);
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

void mouse_pos_interp(glm::dvec2 window_min, glm::dvec2 window_max, glm::dvec2 target_min, glm::dvec2 target_max, glm::dvec2 &mouse_pos)
{
	mouse_pos.x = (target_max.x - target_min.x) / (window_max.x - window_min.x) * (mouse_pos.x - window_min.x) + target_min.x;
	mouse_pos.y = (target_max.y - target_min.y) / (window_max.y - window_min.y) * (mouse_pos.y - window_min.y) + target_min.y;
}

glm::dvec2 get_mouse_pos(gl_instance &gl)
{
	glm::dvec2 mouse_pos;
	glfwGetCursorPos(gl.get_window().handle, &mouse_pos.x, &mouse_pos.y);
		
	glm::ivec2 min = gl.viewport_min();
	glm::ivec2 size = gl.viewport_size();
	mouse_pos_interp({min.x, min.y + size.y}, {min.x + size.x, min.y}, {0, 0}, {target_width, target_height}, mouse_pos);

	return mouse_pos;
}