#include "gl_object.h"
#include "run_game.h"
#include "menu.h"

// returns buttons.size() if no selection
std::size_t select_level_menu(gl_instance &gl, const std::vector<button> &buttons);

int main()
{
	gl_instance gl(target_width, target_height, "Map Jumper");
	const auto &win = gl.get_window();

	menu main_menu(gl, {{target_width * .2, target_height * .1}, {target_width * .6, target_height * .8}}, {"Play Game", "Select Level", "Instructions", "Quit"});

	enum options
	{
		play_game,
		select_level,
		instructions,
		quit,
	};

	auto levels = get_levels("levels");
	std::size_t cur_level = 0;

	// construct and organize level buttons
	std::vector<button> level_buttons;
	{
		glm::vec2 level_button_size{target_width * .1f, target_height * .1f};
		float level_gap = 20;
		float init_y = target_height - level_gap - level_button_size.x / 2;
		glm::vec2 cur_loc{level_gap + level_button_size.x / 2, init_y};
		level_buttons.reserve(levels.size());
		for (std::size_t i = 0; i < levels.size(); ++i)
		{
			level_buttons.emplace_back(gl.get_font(), cur_loc, level_button_size, "Level " + std::to_string(i + 1));
			cur_loc.y -= level_button_size.y + level_gap;
			if (cur_loc.y - level_button_size.y / 2 < level_gap)
			{
				cur_loc.y = init_y;
				cur_loc.x += level_button_size.x + level_gap;
			}
		}
	}

	text level_text(gl.get_font());

	auto set_text = [&](const std::basic_string<std::uint32_t> &str)
	{
		static constexpr float height = 30.f;

		level_text.set_string(str);
		level_text.set_text_scale({1, 1});

		rect level_text_bound = level_text.get_local_rect();
		float scale_diff = height / level_text_bound.dims.y;
		level_text.set_text_scale({scale_diff, scale_diff});

		glm::vec2 desired_text_min{glm::vec2{target_width / 2.f, height / 2.f + 20} - level_text_bound.dims * scale_diff / 2.f};
		level_text.set_text_origin(desired_text_min - level_text_bound.min * scale_diff);
	};
	set_text(level_buttons[cur_level].get_text().get_string());
	
	auto draw_text = [&]() { level_text.draw(gl); };

	while (!glfwWindowShouldClose(win.handle))
	{
		std::size_t option = main_menu.run(gl, draw_text);

		switch ((options)option)
		{
		case play_game:
			if (std::size_t level_change = run_game(gl, std::ranges::subrange(levels.begin() + cur_level, levels.end())))
			{
				cur_level += level_change;
				set_text(level_buttons[cur_level].get_text().get_string());
			}
			break;
		case select_level:
			if (std::size_t select = select_level_menu(gl, level_buttons); select < level_buttons.size())
			{
				cur_level = select;
				set_text(level_buttons[cur_level].get_text().get_string());
			}
			break;
		case instructions:
			message(gl, "Map Jumper Instructions\n"
						"--------------------------------------\n"
						"Map jumper is a simple platforming game.\n"
						"The goal is to reach the end of the level by navigating through the map.\n"
						"You can switch between active blocks by left clicking.\n"
						"Touching the top of spikes will kill you, starting over the level.\n"
						"You can touch the bottom of spikes and not die.\n"
						"You can wall jump off of blocks with arrows.\n"
						"Key Bindings\n"
						"--------------------------------------\n"
						"Left Click: Switch Active Block Color\n"
						"Space: Jump\n"
						"A/D: Move Left/Right\n");
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