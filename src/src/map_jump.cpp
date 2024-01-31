#include "gl_object.h"
#include "run_game.h"
#include "menu.h"

#include <format>

// returns buttons.size() if no selection
std::size_t select_level_menu(gl_instance &gl, const std::vector<button> &buttons);

int main()
{
	gl_instance gl(target_width, target_height, "Map Jumper");
	const auto &win = gl.get_window();

	menu main_menu(gl, {{target_width * .2, target_height * .1}, {target_width * .6, target_height * .8}}, {"Play Game", "Select Level", "Quit"});

	enum options
	{
		play_game,
		select_level,
		quit,
	};

	auto levels = get_levels("levels");
	std::size_t cur_level = 0;

	std::vector<button> level_buttons;
	{
		glm::vec2 level_button_size{target_width * .1f, target_height * .1f};
		float level_gap = 20;
		float init_y = target_height - level_gap - level_button_size.x / 2;
		glm::vec2 cur_loc{level_gap + level_button_size.x / 2, init_y};
		level_buttons.reserve(levels.size());
		for (std::size_t i = 0; i < levels.size(); ++i)
		{
			level_buttons.emplace_back(gl.get_font(), cur_loc, level_button_size, std::format("Level {}", i + 1));
			cur_loc.y -= level_button_size.y + level_gap;
			if (cur_loc.y - level_button_size.y / 2 < level_gap)
			{
				cur_loc.y = init_y;
				cur_loc.x += level_button_size.x + level_gap;
			}
		}
	}

	text level_text(level_buttons[cur_level].get_text().get_string(), leveLgl.get_font());

	while (!glfwWindowShouldClose(win.handle))
	{
		std::size_t option = main_menu.run(gl);

		switch ((options)option)
		{
		case play_game:
			run_game(gl, std::ranges::subrange(levels.begin() + cur_level, levels.end()));
			break;
		case select_level:
			if (std::size_t select = select_level_menu(gl, level_buttons); select < level_buttons.size())
				cur_level = select;
			break;
		case quit:
			return 0;
		}
	}
}

#ifdef _WIN32
int WinMain()
{
	return main();
}
#endif

std::size_t select_level_menu(gl_instance &gl, const std::vector<button> &buttons)
{
	const auto &win = gl.get_window();

	std::size_t select = buttons.size();
	
	auto draw = [&]()
	{
		glm::dvec2 mouse_pos = get_mouse_pos(gl);
		
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		for (std::size_t i = 0; i < buttons.size(); ++i)
		{
			glm::vec4 color;
			if (buttons[i].in_button(mouse_pos))
			{
				if (gl.get_left_click().is_initial_press())
					select = i;
				color = {.2, .2, .2, .2};
			}
			else
				color = {.1, .1, .1, .1};
			
			buttons[i].draw(gl, color);
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

		if (select < buttons.size())
			break;
	}

	return select;
}