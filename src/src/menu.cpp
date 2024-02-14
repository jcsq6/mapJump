#include "menu.h"
#include "button.h"
#include "utility.h"

#include <sstream>

menu::menu(gl_instance &gl, const rect &space, const std::vector<std::string> &options)
{
	m_options.reserve(options.size());

	float window_button_usable_height = space.dims.y;
	float button_total_height = window_button_usable_height / options.size();
	float button_gap = button_total_height * .2f;
	glm::vec2 button_dims{space.dims.x, button_total_height * .8f};
	glm::vec2 loc = space.min;
	loc.x += space.dims.x / 2;
	loc.y += space.dims.y;

	for (std::size_t i = 0; i < options.size(); ++i)
	{
		m_options.emplace_back(gl.get_font(), glm::vec2{loc.x, loc.y - button_dims.y / 2}, button_dims, options[i]);
		loc.y -= button_total_height;
	}
}

std::size_t menu::run(gl_instance &gl, std::function<void()> extra_draw) const
{
	const auto &win = gl.get_window();

	std::size_t option = m_options.size();

	auto draw = [&]()
	{
		glm::dvec2 mouse_pos = get_mouse_pos(gl);

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		if (extra_draw)
			extra_draw();

		for (std::size_t i = 0; i < m_options.size(); ++i)
		{
			glm::vec4 color;
			if (m_options[i].in_button(mouse_pos))
			{
				if (gl.get_left_click().is_initial_press())
					option = i;
				color = {.2, .2, .2, .2};
			}
			else
				color = {.1, .1, .1, .1};
			
			m_options[i].draw(gl, color);
		}

		glfwSwapBuffers(win.handle);
	};

	gl.register_draw_function(draw);


	while (!glfwWindowShouldClose(win.handle))
	{
		glfwWaitEvents();

		gl.get_escape_key().update(glfwGetKey(win.handle, GLFW_KEY_ESCAPE));
		if (gl.get_escape_key().is_initial_press())
			break;
		
		gl.get_left_click().update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));

		draw();

		if (option < m_options.size())
			break;
	}

	return option;
}

rect messsage_bound(gl_instance &gl, const std::string &message)
{
	constexpr float line_spacing = 10;
	constexpr float text_height = 20;
	float y = target_height - line_spacing - text_height / 2;
	
	std::istringstream stream(message);
	std::string line;

	glm::vec2 min{std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()};
	glm::vec2 max{-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
	while (std::getline(stream, line))
	{
		if (line.empty())
			continue;
		
		text txt(line, gl.get_font());
		rect bound = txt.get_local_rect();

		float scale_diff = text_height / bound.dims.y;
		bound.min *= scale_diff;
		bound.dims *= scale_diff;

		glm::vec2 center{target_width / 2.f, y};
		glm::vec2 text_min{center - bound.dims / 2.f};
		
		if (text_min.x < min.x)
			min.x = text_min.x;
		if (text_min.y < min.y)
			min.y = text_min.y;
		if (float pot = text_min.x + bound.dims.x; pot > max.x)
			max.x = pot;
		if (float pot = text_min.y + bound.dims.y; pot > max.y)
			max.y = pot;

		y -= bound.dims.y + line_spacing;
	}

	return {min, max - min};
}

void draw_message(gl_instance &gl, const std::string &message)
{
	rect message_bound = messsage_bound(gl, message);
	glm::vec2 desired_dims{target_width * .9f, target_height * .75f};
	glm::vec2 message_scale_diff = desired_dims / message_bound.dims;
	if (message_scale_diff.x < message_scale_diff.y)
		message_scale_diff.y = message_scale_diff.x;
	else
		message_scale_diff.x = message_scale_diff.y;

	float line_spacing = 10 * message_scale_diff.y;
	float text_height = 20 * message_scale_diff.y;
	float y = target_height - line_spacing - text_height / 2;
	std::istringstream stream(message);
	std::string line;
	while (std::getline(stream, line))
	{
		if (line.empty())
			continue;
		
		text txt(line, gl.get_font());
		rect bound = txt.get_local_rect();
		float scale_diff = text_height / bound.dims.y;
		txt.set_text_scale({scale_diff, scale_diff});
		bound.min *= scale_diff;
		bound.dims *= scale_diff;

		glm::vec2 center{target_width / 2.f, y};
		glm::vec2 desired_text_min{center - bound.dims / 2.f};
		txt.set_text_origin(desired_text_min - bound.min);

		txt.draw(gl);

		y -= bound.dims.y + line_spacing;
	}
}

bool yes_no(gl_instance &gl, const std::string &message)
{
	const auto &win = gl.get_window();

	button yes(gl.get_font(), {target_width / 2.f - target_width * .1, target_height * .15f}, {target_width * .1f, target_width * .05f}, "Yes");
	button no(gl.get_font(), {target_width / 2.f + target_width * .1, target_height * .15f}, {target_width * .1f, target_width * .05f}, "No");

	std::size_t option = 2;
	auto draw = [&]()
	{
		glm::dvec2 mouse_pos = get_mouse_pos(gl);

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_message(gl, message);

		glm::vec4 color;
		if (no.in_button(mouse_pos))
		{
			if (gl.get_left_click().is_initial_press())
				option = 0;
			color = {.2, .2, .2, .2};
		}
		else
			color = {.1, .1, .1, .1};
		
		no.draw(gl, color);

		if (yes.in_button(mouse_pos))
		{
			if (gl.get_left_click().is_initial_press())
				option = 1;
			color = {.2, .2, .2, .2};
		}
		else
			color = {.1, .1, .1, .1};
		
		yes.draw(gl, color);

		glfwSwapBuffers(win.handle);
	};

	gl.register_draw_function(draw);

	while (true)
	{
		glfwWaitEvents();
		gl.get_left_click().update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));
	
		draw();

		if (option < 2)
			return option;
	}
}

void message(gl_instance &gl, const std::string &message)
{
	const auto &win = gl.get_window();

	button ok(gl.get_font(), {target_width / 2.f, target_height * .15f}, {target_width * .1f, target_width * .05f}, "OK");

	bool pressed = false;
	auto draw = [&]()
	{
		glm::dvec2 mouse_pos = get_mouse_pos(gl);

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		draw_message(gl, message);

		glm::vec4 color;
		if (ok.in_button(mouse_pos))
		{
			if (gl.get_left_click().is_initial_press())
				pressed = true;
			color = {.2, .2, .2, .2};
		}
		else
			color = {.1, .1, .1, .1};

		ok.draw(gl, color);

		glfwSwapBuffers(win.handle);
	};

	gl.register_draw_function(draw);

	while (true)
	{
		glfwWaitEvents();
		gl.get_escape_key().update(glfwGetKey(win.handle, GLFW_KEY_ESCAPE));
		if (gl.get_escape_key().is_initial_press())
			break;
		gl.get_left_click().update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));
	
		draw();

		if (pressed)
			break;
	}
}