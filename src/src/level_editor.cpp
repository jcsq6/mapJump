#include "gl_object.h"

#include "run_game.h"
#include "utility.h"

#include <string>
#include <iostream>
#include <limits>
#include <thread>

#include <tinyfiledialogs.h>

bool yes_no(const std::string &prompt)
{
	int final_choice = -1;
	while (final_choice == -1)
	{
		std::cout << prompt << " (y/n)? ";
		char choice;
		std::cin >> choice;

		if (choice == 'y' || choice == 'Y')
			final_choice = 1;
		else if (choice == 'n' || choice == 'N')
			final_choice = 0;
		else
			std::cout << "Invalid input.\n";

		std::cin.clear();
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	return final_choice;
}

void run_level_editor(level &l, bool &has_spawn, bool &has_end);

int main()
{
	std::string cwd = std::filesystem::current_path().string();
	cwd += '/';
	const char *const filter = "*.lvl";
	while (true)
	{
		std::cout << "Options:\n"
					 "  1. Create New Level\n"
					 "  2. Load Level\n"
					 "  3. Instructions\n"
					 "  4. Play level\n"
					 "  5. Quit\n"
					 "Input a Selection: ";
		unsigned int option;
		while (true)
		{
			std::cin >> option;
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			if (std::cin.fail() || option > 5 || option == 0)
				std::cout << "Invalid option.\n";
			else
				break;
		}


		std::filesystem::path filename;
		level l;

		switch (option)
		{
		case 1:
		{
			l.construct_default();
			break;
		}
		case 2:
		case 4:
		{
			std::cout << "Select file...\n";

			const char *path = tinyfd_openFileDialog("Select Level", cwd.c_str(), 1, &filter, "Level file", 0);
			if (!path)
			{
				std::cout << "Cancelling...\n";
				continue;
			}
			filename = path;

			if (!std::filesystem::exists(filename))
				std::cout << "File not found.\n";
			else if (filename.extension() != ".lvl")
				std::cout << "Wrong file type.\n";
			else
			{
				try
				{
					l.read_level(filename);
					break;
				}
				catch (const std::runtime_error &e)
				{
					std::cout << "Failed to read level: " << e.what() << '\n';
				}
			}

			continue; // if it got here, it failed
		}
		case 3:
			std::cout << "\nLevel Editor Instructions\n"
						 "--------------------------------------\n"
						 "  Left Click: Place Block\n"
						 "  Right Click: Remove Block\n"
						 "  P/Middle Click: Pick Block\n"
						 "  Scroll/Space: Scroll Block Type\n"
						 "  W/Up: Scroll Color Up\n"
						 "  S/Down: Scroll Color Down\n"
						 "  A/Left: Rotate CCW\n"
						 "  D/Right: Rotate CW\n"
						 "Tips\n"
						 "--------------------------------------\n"
						 "  You can place multiple spikes in the same block.\n"
						 "  Use pick block to speed up the process.\n"
						 "  Use the Spawn Anchor (green with anchor) to set spawn location (Required).\n"
						 "  Use End Anchor (red with anchor) to set level end location (Required).\n"
						 "  Load level option will automatically save the level at the end.\n"
						 "  For levels to work with the game, keep the outside wall and add an opening that will lead into another level.\n"
						 "  Level exits can be anywhere on the edge, just make sure that it matches the opening to the next level.\n"
						 "  Consecutive levels' names should end with _*number* where number is in order for it to work with the game.\n"
						 "Press enter to continue...\n";
			{
				std::string dummy;
				std::getline(std::cin, dummy);	
			}
			continue;
		case 5:
			return 0;
		}

		// play level
		if (option == 4)
		{
			run_game(std::vector<level>{l});
			std::cout << '\n';
			continue;
		}

		bool has_spawn = true;
		bool has_end = true;

		while (true)
		{
			run_level_editor(l, has_spawn, has_end);
			if (!has_spawn || !has_end)
			{
				std::cout << '\n';
				if (!has_spawn)
					std::cout << "Spawn anchor is not set!\n";
				if (!has_end)
					std::cout << "End anchor is not set!\n";
				std::cout << "Press enter to re-open level...\n";
				std::string dummy;
				std::getline(std::cin, dummy);
			}
			else
				break;
		}

		while (yes_no("Play level"))
		{
			run_game(std::vector<level>{l});

			if (yes_no("Continue editing"))
				run_level_editor(l, has_spawn, has_end);
			else
				break;
		}

		if (!yes_no("Save level"))
			continue;

		bool save = true;

		// if they created a level
		if (option == 1)
		{
			const char *tmp;
			while (true)
			{
				tmp = tinyfd_saveFileDialog("Save Level", cwd.c_str(), 1, &filter, "Level File");
				if (!tmp)
				{
					std::cout << "Failed...\n";
					if (!yes_no("Try again"))
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
					std::cout << "Successfully saved level " << filename.filename() << "\n";
					break;
				}
				catch(const std::exception& e)
				{
					std::cout << "Failed to save level " << filename.filename() << ": " << e.what() << '\n';
					if (yes_no("Try again"))
					{
						using namespace std::chrono_literals;
						std::cout << "Waiting 5 seconds...\n";
						std::this_thread::sleep_for(5s);
						continue;
					}
					else
						break;
				}
			}
	}
}

void mouse_pos_interp(glm::dvec2 window_min, glm::dvec2 window_max, glm::dvec2 target_min, glm::dvec2 target_max, glm::dvec2 &mouse_pos)
{
	mouse_pos.x = (target_max.x - target_min.x) / (window_max.x - window_min.x) * (mouse_pos.x - window_min.x) + target_min.x;
	mouse_pos.y = (target_max.y - target_min.y) / (window_max.y - window_min.y) * (mouse_pos.y - window_min.y) + target_min.y;
}

// loc is location on target screen
auto find_block(level &l, glm::vec2 loc)
{
	polygon_view poly(square(), loc, {1, 1}, 0);
	auto it = l.blocks.begin();
	for (; it < l.blocks.end(); ++it)
		if (collides(poly, it->poly))
			return it;
	return it;
}

block::type current_type;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
	int next = (static_cast<char>(current_type) + static_cast<int>(yoffset)) % 5;
	if (next < 0)
		next += 5;
	current_type = static_cast<block::type>(next);
}

glm::ivec2 window_size;

void window_size_callback(GLFWwindow* window, int width, int height)
{
	window_size.x = width;
	window_size.y = height;
}

void run_level_editor(level &l, bool &has_spawn, bool &has_end)
{
	auto ortho = glm::ortho<float>(0, (float)target_width, 0, (float)target_height, -1, 1);

	gl_instance gl(target_width, target_height, "Level Editor");
	const window &win = gl.get_window();
	const auto &program = gl.get_texture_program();

	glfwSetScrollCallback(win.handle, scroll_callback);
	glfwSetWindowSizeCallback(win.handle, window_size_callback);
	glfwSetFramebufferSizeCallback(win.handle, framebuffer_size_callback);

	glfwGetWindowSize(win.handle, &window_size.x, &window_size.y);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	color current_color = color::neutral;
	direction current_dir = direction::up;
	current_type = block::type::normal;

	block spawn_anchor(l.start, block::type::spawn_anchor, {}, {});
	block end_anchor(l.end, block::type::end_anchor, {}, {});

	key left_click;
	key right_click;
	key pick_block;
	key color_up;
	key color_down;
	key angle_right;
	key angle_left;
	key type_up;

	while (!glfwWindowShouldClose(win.handle))
	{
		glfwWaitEvents();

		if (glfwGetKey(win.handle, GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(win.handle, GLFW_TRUE);

		left_click.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));
		right_click.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_RIGHT));
		pick_block.update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_MIDDLE) || glfwGetKey(win.handle, GLFW_KEY_P));
		type_up.update(glfwGetKey(win.handle, GLFW_KEY_SPACE));
		color_up.update(glfwGetKey(win.handle, GLFW_KEY_W) || glfwGetKey(win.handle, GLFW_KEY_UP));
		color_down.update(glfwGetKey(win.handle, GLFW_KEY_S) || glfwGetKey(win.handle, GLFW_KEY_DOWN));
		angle_right.update(glfwGetKey(win.handle, GLFW_KEY_D) || glfwGetKey(win.handle, GLFW_KEY_RIGHT));
		angle_left.update(glfwGetKey(win.handle, GLFW_KEY_A) || glfwGetKey(win.handle, GLFW_KEY_LEFT));

		glm::dvec2 mouse_pos;
		glfwGetCursorPos(win.handle, &mouse_pos.x, &mouse_pos.y);

		mouse_pos_interp({0, window_size.y}, {window_size.x, 0}, {0, 0}, {target_width, target_height}, mouse_pos);

		glm::ivec2 grid_pos{mouse_pos / (double)game::block_size};

		block current_block(grid_pos, current_type, current_color, current_dir);

		if (left_click.is_initial_press())
		{
			if (grid_pos == l.start)
				has_spawn = false;
			else if (grid_pos == l.end)
				has_end = false;
			else
			{
				auto it = find_block(l, current_block.poly.offset);
				if (it != l.blocks.end())
					l.blocks.erase(it);
			}

			if (current_type == block::type::spawn_anchor)
			{
				l.start = grid_pos;
				has_spawn = true;
				spawn_anchor = block(l.start, block::type::spawn_anchor, {}, {});
			}
			else if (current_type == block::type::end_anchor)
			{
				l.end = grid_pos;
				has_end = true;
				end_anchor = block(l.end, block::type::end_anchor, {}, {});
			}
			else
				l.blocks.push_back(current_block);
		}

		if (right_click.is_initial_press())
		{
			if (grid_pos == l.start)
				has_spawn = false;
			else if (grid_pos == l.end)
				has_end = false;
			else if (auto it = find_block(l, current_block.poly.offset); it != l.blocks.end())
				l.blocks.erase(it);
		}
		
		if (pick_block.is_initial_press())
		{
			if (grid_pos == l.start)
			{
				current_type = block::type::spawn_anchor;
				current_block = block(grid_pos, block::type::spawn_anchor, {}, {});
				current_color = color::neutral;
			}
			else if (grid_pos == l.end)
			{
				current_type = block::type::end_anchor;
				current_block = block(grid_pos, block::type::end_anchor, {}, {});
				current_color = color::neutral;
			}
			else if (auto it = find_block(l, mouse_pos); it != l.blocks.end())
			{
				current_dir = it->dir();
				current_color = it->block_color;
				current_type = it->block_type;
			}
		}

		if (type_up.is_initial_press())
			current_type = static_cast<block::type>((static_cast<int>(current_type) + 1) % 5);

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

		l.draw(color::no_color, gl);
		if (has_spawn)
			spawn_anchor.draw(color::no_color, gl);
		if (has_end)
			end_anchor.draw(color::no_color, gl);
		current_block.draw(color::no_color, gl);

		glfwSwapBuffers(win.handle);
	}
}