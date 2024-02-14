#include "gl_object.h"

#include "run_game.h"
#include "utility.h"

#include "button.h"

#include "menu.h"

#include <string>
#include <limits>
#include <thread>

#include <tinyfiledialogs.h>

void run_level_editor(gl_instance &gl, level &l);

int main()
{
	std::string cwd = std::filesystem::current_path().string();
	cwd += '/';
	const char *const filter = "*.lvl";

	gl_instance gl(target_width, target_height, "Level Editor");
	const window &win = gl.get_window();

	menu main_menu(gl, {{target_width * .2, target_height * .1}, {target_width * .6, target_height * .8}}, {"Create New Level", "Load Level", "Play Level", "Instructions", "Quit"});

	enum option_type
	{
		create_level = 0,
		load_level = 1,
		play_level = 2,
		instructions = 3,
		quit = 4,
	};

	while (!glfwWindowShouldClose(win.handle))
	{
		std::size_t option = main_menu.run(gl);

		// escape pressed
		if (option >= main_menu.size())
			break;

		std::filesystem::path filename;
		level l;

		switch ((option_type)option)
		{
		case create_level:
		{
			l.construct_default();
			break;
		}
		case load_level:
		case play_level:
		{
			const char *path = tinyfd_openFileDialog("Select Level", cwd.c_str(), 1, &filter, "Level file", 0);
			if (!path)
				continue;
			
			filename = path;

			if (!std::filesystem::exists(filename))
				message(gl, "File not found.");
			else if (filename.extension() != ".lvl")
				message(gl, "Wrong file type.");
			else
			{
				try
				{
					l.read_level(filename);
					break;
				}
				catch (const std::runtime_error &e)
				{
					message(gl, std::string("Failure to read level: ") + e.what());
				}
			}

			continue; // if it got here, it failed
		}
		case instructions:
			message(gl, "Level Editor Instructions\n"
						"--------------------------------------\n"
						"Left Click: Place Block\n"
						"Right Click: Remove Block\n"
						"P/Middle Click: Pick Block\n"
						"Scroll/Space: Scroll Block Type\n"
						"W/Up: Scroll Color Up\n"
						"S/Down: Scroll Color Down\n"
						"A/Left: Rotate CCW\n"
						"D/Right: Rotate CW\n"
						"Tips\n"
						"--------------------------------------\n"
						"You can place multiple spikes in the same block.\n"
						"Use pick block to speed up the process.\n"
						"You can click and drag place block and erase block for quicker editing.\n"
						"You can use the bottom of a spike as a floor or ceiling and the player won't die.\n"
						"After initial editing is finished, place spawn anchor (green), and end anchor (red).\n"
						"For levels to work with the game, keep the outside wall and add an opening that will lead into another level.\n"
						"Consecutive levels' names should end with _*number* where number is in order for it to work with the game.\n");
			continue;
		case quit:
			return 0;
		}

		// play level
		if (option == play_level)
		{
			run_game(gl, std::ranges::single_view{l});
			continue;
		}

		run_level_editor(gl, l);

		while (yes_no(gl, "Play level?"))
		{
			run_game(gl, std::ranges::single_view{l});

			if (yes_no(gl, "Continue editing?"))
				run_level_editor(gl, l);
			else
				break;
		}

		if (!yes_no(gl, "Save level?"))
			continue;

		bool save = true;

		// if they created a level
		if (option == create_level)
		{
			const char *tmp;
			while (true)
			{
				tmp = tinyfd_saveFileDialog("Save Level", cwd.c_str(), 1, &filter, "Level File");
				if (!tmp)
				{
					if (!yes_no(gl, "Failed...\nTry again?"))
					{
						save = false;
						break;
					}
				}
				else
					break;
			}
			filename = tmp;
			filename.replace_extension(".lvl");
		}

		if (save)
			while (true)
			{
				try
				{
					l.write_level(filename);
					message(gl, "Successfully saved level " + filename.filename().string() + '\n');
					break;
				}
				catch(const std::exception& e)
				{
					if (yes_no(gl, "Failed to save level " + filename.filename().string() + "\nError: " + e.what() + "\nTry again?"))
					{
						using namespace std::chrono_literals;
						std::this_thread::sleep_for(5s);
						continue;
					}
					else
						break;
				}
			}
	}

	return 0;
}

#ifdef _WIN32
int WinMain()
{
	return main();
}
#endif

// loc is location on target screen
auto find_block(level &l, glm::vec2 loc)
{
	polygon_view poly(square(), loc, {10, 10}, 0);
	auto it = l.blocks.begin();
	for (; it < l.blocks.end(); ++it)
		if (collides(poly, it->poly))
			return it;
	return it;
}

block::type current_type;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	int next = (static_cast<char>(current_type) + static_cast<int>(yoffset)) % 3;
	if (next < 0)
		next += 3;
	current_type = static_cast<block::type>(next);
}

enum class spawn_or_end
{
	spawn,
	end,
};

spawn_or_end end_block_type;
void spawn_end_scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	int next = (static_cast<char>(current_type) + static_cast<int>(yoffset)) % 2;
	if (next < 0)
		next += 2;
	end_block_type = static_cast<spawn_or_end>(next);
}

void run_level_editor(gl_instance &gl, level &l)
{
	const auto &win = gl.get_window();

	glfwSetScrollCallback(win.handle, scroll_callback);

	// enable transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	color current_color = color::neutral;
	direction current_dir = direction::up;
	current_type = block::type::normal;

	block current_block;

	auto draw = [&]()
	{
		const auto &program = gl.get_texture_program();

		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// set uniforms
		glUseProgram(program.id);
		glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &gl.get_ortho()[0][0]);

		print_background(gl);

		l.draw(color::no_color, gl);
		current_block.draw(color::no_color, gl, .75f);

		glfwSwapBuffers(win.handle);
	};

	gl.register_draw_function(draw);

	key right_click;
	key pick_block;
	key color_up;
	key color_down;
	key angle_right;
	key angle_left;
	key type_up;

	bool has_spawn = true;
	bool has_end = true;

	// so that a block is not immediately placed when entering level editor
	bool left_released = false;

	while (!glfwWindowShouldClose(win.handle))
	{
		glfwWaitEvents();

		gl.get_escape_key().update(glfwGetKey(win.handle, GLFW_KEY_ESCAPE));
		if (gl.get_escape_key().is_initial_press())
			break;
		
		gl.get_left_click().update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));
		if (!gl.get_left_click().is_pressed())
			left_released = true;
		
		right_click.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_RIGHT));
		pick_block.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_MIDDLE) || glfwGetKey(win.handle, GLFW_KEY_P));
		type_up.update(glfwGetKey(win.handle, GLFW_KEY_SPACE));
		color_up.update(glfwGetKey(win.handle, GLFW_KEY_W) || glfwGetKey(win.handle, GLFW_KEY_UP));
		color_down.update(glfwGetKey(win.handle, GLFW_KEY_S) || glfwGetKey(win.handle, GLFW_KEY_DOWN));
		angle_right.update(glfwGetKey(win.handle, GLFW_KEY_D) || glfwGetKey(win.handle, GLFW_KEY_RIGHT));
		angle_left.update(glfwGetKey(win.handle, GLFW_KEY_A) || glfwGetKey(win.handle, GLFW_KEY_LEFT));

		glm::dvec2 mouse_pos = get_mouse_pos(gl);

		glm::ivec2 grid_pos{mouse_pos / (double)game::block_size};

		current_block = block(grid_pos, current_type, current_color, current_dir);
		
		// place block (if in bounds)
		if (left_released && gl.get_left_click().is_pressed() && grid_pos.x >= 0 && grid_pos.x < game::map_width && grid_pos.y >= 0 && grid_pos.y < game::map_height)
		{
			if (has_spawn && grid_pos == l.start)
				has_spawn = false;
			else if (has_end && grid_pos == l.end)
				has_end = false;
			// could be in an else but I'll leave it out for a failsafe
			// erase any blocks that were there before
			while (true)
			{
				auto it = find_block(l, current_block.poly.offset);
				if (it != l.blocks.end())
					l.blocks.erase(it);
				else
					break;
			}

			l.blocks.push_back(current_block);
		}

		// remove block
		if (right_click.is_pressed())
		{
			// erase block at mouse pos
			auto it = find_block(l, mouse_pos);
			if (it != l.blocks.end())
				l.blocks.erase(it);
		}
		
		// pick block
		if (pick_block.is_initial_press())
		{
			if (auto it = find_block(l, mouse_pos); it != l.blocks.end())
			{
				current_dir = it->dir();
				current_color = it->block_color;
				current_type = it->block_type;
			}
		}

		if (type_up.is_initial_press())
			current_type = static_cast<block::type>((static_cast<int>(current_type) + 1) % 3);

		if (color_up.is_initial_press())
			current_color = static_cast<color>((static_cast<char>(current_color) + 1) % 3);
		if (color_down.is_initial_press())
			current_color = static_cast<color>((static_cast<char>(current_color) + 2) % 3);
		
		if (angle_right.is_initial_press())
			current_dir = static_cast<direction>((static_cast<char>(current_dir) + 1) % 4);
		if (angle_left.is_initial_press())
			current_dir = static_cast<direction>((static_cast<char>(current_dir) + 3) % 4);

		draw();
	}

	left_released = false;

	// true for placing spawn anchor, false for placing end anchor
	end_block_type = spawn_or_end::spawn;

	glm::ivec2 grid_pos;

	auto draw_2 = [&]()
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		const auto &program = gl.get_texture_program();

		// set uniforms
		glUseProgram(program.id);
		glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &gl.get_ortho()[0][0]);

		print_background(gl);

		l.draw(color::no_color, gl);
		
		glBindVertexArray(gl.get_shapes().square_vao().id);
		
		if (has_spawn)
		{
			glm::vec2 loc = l.start * game::block_size;
			loc.x += game::block_size / 2;
			loc.y += game::block_size / 2;

			auto m = glm::scale(glm::translate(glm::mat4(1.f), {loc, 0}), {game::block_size, game::block_size, 0});
			glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);

			glBindTexture(GL_TEXTURE_2D, gl.get_assets().spawn_anchor.id);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		if (has_end)
		{
			glm::vec2 loc = l.end * game::block_size;
			loc.x += game::block_size / 2;
			loc.y += game::block_size / 2;

			auto m = glm::scale(glm::translate(glm::mat4(1.f), {loc, 0}), {game::block_size, game::block_size, 0});
			glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);

			glBindTexture(GL_TEXTURE_2D, gl.get_assets().end_anchor.id);
			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		}

		const texture *text = end_block_type == spawn_or_end::end ? &gl.get_assets().end_anchor : &gl.get_assets().spawn_anchor;
		glm::vec2 loc = grid_pos * game::block_size;
		loc.x += game::block_size / 2;
		loc.y += game::block_size / 2;
		auto m = glm::scale(glm::translate(glm::mat4(1.f), {loc, 0}), {game::block_size, game::block_size, 0});
		glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);
		glUniform1f(glGetUniformLocation(gl.get_texture_program().id, "transparency"), .75f);

		glBindTexture(GL_TEXTURE_2D, text->id);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glfwSwapBuffers(win.handle);
	};

	gl.register_draw_function(draw_2);
	glfwSetScrollCallback(win.handle, spawn_end_scroll_callback);

	// place spawn and end anchor
	while (true)
	{
		glfwWaitEvents();

		if (glfwWindowShouldClose(win.handle))
		{
			if (!has_spawn || !has_end)
			{
				message(gl, "Must place spawn and end anchors!");
				glfwSetWindowShouldClose(win.handle, false);
				gl.register_draw_function(draw_2);
			}
			else
				break;
		}

		gl.get_escape_key().update(glfwGetKey(win.handle, GLFW_KEY_ESCAPE));
		if (gl.get_escape_key().is_initial_press())
		{
			if (!has_spawn || !has_end)
			{
				message(gl, "Must place spawn and end anchors!");
				gl.register_draw_function(draw_2);
			}
			else
				break;
		}
		
		gl.get_left_click().update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));
		if (!gl.get_left_click().is_pressed())
			left_released = true;

		right_click.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_RIGHT));

		type_up.update(glfwGetKey(win.handle, GLFW_KEY_SPACE));
		if (type_up.is_initial_press())
			end_block_type = static_cast<spawn_or_end>((static_cast<char>(end_block_type) + 1) % 2);

		glm::dvec2 mouse_pos = get_mouse_pos(gl);
		grid_pos = {mouse_pos / (double)game::block_size};

		if (left_released && gl.get_left_click().is_initial_press())
		{
			if (has_spawn && grid_pos == l.start)
				has_spawn = false;
			else if (has_end && grid_pos == l.end)
				has_end = false;
			else
			{
				// erase any blocks that were there before
				while (true)
				{
					auto it = find_block(l, grid_pos * game::block_size + glm::ivec2(game::block_size, game::block_size) / 2);
					if (it != l.blocks.end())
						l.blocks.erase(it);
					else
						break;
				}
			}

			if (end_block_type == spawn_or_end::spawn)
			{
				has_spawn = true;
				l.start = grid_pos;
			}
			else
			{
				l.end = grid_pos;
				has_end = true;
			}
		}

		if (right_click.is_pressed())
		{
			if (has_spawn && l.start == grid_pos)
				has_spawn = false;
			else if (has_end && l.end == grid_pos)
				has_end = false;
		}

		draw_2();
	}

	glfwSetScrollCallback(win.handle, nullptr);
}