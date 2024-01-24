#include "gl_object.h"

#include "run_game.h"
#include "utility.h"

#include <string>
#include <iostream>

void mouse_pos_interp(glm::dvec2 window_min, glm::dvec2 window_max, glm::dvec2 target_min, glm::dvec2 target_max, glm::dvec2 &mouse_pos)
{
	mouse_pos.x = (target_max.x - target_min.x) / (window_max.x - window_min.x) * (mouse_pos.x - window_min.x) + target_min.x;
	double y = window_max.y - mouse_pos.y;
	mouse_pos.y = (target_max.y - target_min.y) / (window_max.y - window_min.y) * (y - window_min.y) + target_min.y;
}

// loc is location on target screen
auto find_block(level &l, glm::vec2 loc)
{
	polygon_view poly(shapes::instance().get().square, loc, {1, 1}, 0);
	auto it = l.blocks.begin();
	for (; it < l.blocks.end(); ++it)
		if (collides(poly, it->poly))
			return it;
	return it;
}

void run_level_editor(level &l, bool reset);

int main()
{
	while (true)
	{
		std::cout << "Create level (y/n)? ";
		char answer;
		std::cin >> answer;
		if (answer == 'y' || answer == 'Y')
		{
			level new_level;
			run_level_editor(new_level, true);
		}
		else if (answer == 'n' || answer == 'N')
		{
			// load level prompt
		}
		else
		{
			std::cout << "Invalid answer.\n";
		}
	}
}

block::type current_type;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	int next = (static_cast<char>(current_type) + static_cast<int>(yoffset)) % 3;
	current_type = static_cast<block::type>(next);
}

void run_level_editor(level &l, bool reset)
{
	auto ortho = glm::ortho<float>(0, (float)target_width, 0, (float)target_height, -1, 1);

	gl_instance::instance().create_window(window_width, window_height, "Level Editor");
	window &win = gl_instance::instance().get_window();
	const auto &program = texture_program().get();

	glfwSetScrollCallback(win.handle, scroll_callback);

	if (reset)
		l.construct_default();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	color current_color = color::neutral;
	direction current_dir = direction::right;
	current_type = block::type::spike;

	key left_click;
	key right_click;
	key color_up;
	key color_down;
	key angle_right;
	key angle_left;

	while (!glfwWindowShouldClose(win.handle))
	{
		glfwWaitEvents();

		if (glfwGetKey(win.handle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(win.handle, GLFW_TRUE);

		left_click.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
		right_click.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
		color_up.update(glfwGetKey(win.handle, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(win.handle, GLFW_KEY_UP) == GLFW_PRESS);
		color_down.update(glfwGetKey(win.handle, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(win.handle, GLFW_KEY_DOWN) == GLFW_PRESS);
		angle_right.update(glfwGetKey(win.handle, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(win.handle, GLFW_KEY_RIGHT) == GLFW_PRESS);
		angle_left.update(glfwGetKey(win.handle, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(win.handle, GLFW_KEY_LEFT) == GLFW_PRESS);

		glm::dvec2 mouse_pos;
		glfwGetCursorPos(win.handle, &mouse_pos.x, &mouse_pos.y);
		mouse_pos_interp({0, 0}, {window_width, window_height}, {0, 0}, {target_width, target_height}, mouse_pos);

		glm::ivec2 grid_pos{mouse_pos / (double)game::block_size};

		block current_block(grid_pos, current_type, current_color, current_dir);

		if (left_click.is_initial_press())
		{
			auto it = find_block(l, current_block.poly.offset);
			if (it != l.blocks.end())
				l.blocks.erase(it);
			l.blocks.push_back(current_block);
		}

		if (right_click.is_initial_press())
			if (auto it = find_block(l, mouse_pos); it != l.blocks.end())
				l.blocks.erase(it);

		if (color_up.is_initial_press())
			current_color = static_cast<color>((static_cast<char>(current_color) + 1) % 3);
		if (color_down.is_initial_press())
			current_color = static_cast<color>((static_cast<char>(current_color) + 2) % 3);
		
		if (angle_right.is_initial_press())
			current_dir = static_cast<direction>((static_cast<char>(current_dir) + 1) % 4);
		if (angle_left.is_initial_press())
			current_dir = static_cast<direction>((static_cast<char>(current_dir) + 3) % 4);

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// set uniforms
		glUseProgram(program.id);
		glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &ortho[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(program.id, "text"), 0);

		l.draw(color::no_color);
		current_block.draw(color::no_color);

		glfwSwapBuffers(win.handle);
	}

	gl_instance::instance().destroy_window();
}