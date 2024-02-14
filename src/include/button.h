#ifndef BUTTON_H
#define BUTTON_H
#include "text.h"

class button
{
public:
	button(font &_font, glm::vec2 center, glm::vec2 dims, const std::string &text) : m_text(_font)
	{
		m_text.set_string<char>(text);
		auto cur_bounds = m_text.get_local_rect();
		glm::vec2 scale_diff = dims / cur_bounds.dims;
		if (scale_diff.x < scale_diff.y)
			scale_diff.y = scale_diff.x;
		else
			scale_diff.x = scale_diff.y;
		scale_diff *= .9;
		
		m_text.set_text_scale(scale_diff);

		cur_bounds.min *= scale_diff;
		cur_bounds.dims *= scale_diff;

		glm::vec2 desired_text_min{center - cur_bounds.dims / 2.f};
		m_text.set_text_origin(desired_text_min - cur_bounds.min);

		m_box.min = center - dims / 2.f;
		m_box.dims = dims;
	}

	void draw(gl_instance &gl, const glm::vec4 &background_color) const
	{
		const auto &program = gl.get_shape_program();

		// set uniforms
		glUseProgram(program.id);
		glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &gl.get_ortho()[0][0]);
		glUniform4fv(glGetUniformLocation(program.id, "color"), 1, &background_color[0]);

		auto m = glm::scale(glm::translate(glm::mat4(1.f), {m_box.min + m_box.dims / 2.f, 0}), {m_box.dims, 0});
		glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);

		glBindVertexArray(gl.get_shapes().square_vao().id);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		m_text.draw(gl);
	}

	bool in_button(glm::vec2 mouse_loc) const
	{
		auto max = m_box.max();
		return mouse_loc.x > m_box.min.x && mouse_loc.x < max.x &&
			   mouse_loc.y > m_box.min.y && mouse_loc.y < max.y;
	}

	const auto &get_text() const { return m_text; }
private:
	text m_text;
	rect m_box;
};

#endif