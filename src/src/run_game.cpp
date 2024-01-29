#include "run_game.h"
#include "utility.h"

int run_game(gl_instance &gl, std::vector<level> &&levels)
{
	constexpr int target_fps = 60;
	constexpr auto target_frame_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::duration<double>(1.0 / target_fps));

	const auto &program = gl.get_texture_program();
	const auto &win = gl.get_window();

	glfwSetFramebufferSizeCallback(win.handle, gl_instance::framebuffer_size_callback);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	game my_game(target_width, target_height, std::move(levels));

	auto draw = [&]()
	{
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		// set uniforms
		glUseProgram(program.id);
		glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &gl.get_ortho()[0][0]);

		my_game.draw(gl);

		glfwSwapBuffers(win.handle);
	};

	gl.register_draw_function(draw);

	key space;

	while (!glfwWindowShouldClose(win.handle))
	{
		std::chrono::time_point frame_begin = std::chrono::steady_clock::now();

		glfwPollEvents();

		gl.get_escape_key().update(glfwGetKey(win.handle, GLFW_KEY_ESCAPE));

		if (gl.get_escape_key().is_initial_press())
			break;
		if (glfwGetKey(win.handle, GLFW_KEY_A))
			my_game.move_left();
		if (glfwGetKey(win.handle, GLFW_KEY_D))
			my_game.move_right();
        
        space.update(glfwGetKey(win.handle, GLFW_KEY_SPACE));
        gl.get_left_click().update(glfwGetMouseButton(win.handle, GLFW_MOUSE_BUTTON_LEFT));

		if (space.is_initial_press())
			my_game.jump();
		if (gl.get_left_click().is_initial_press())
			my_game.switch_colors();

		my_game.update(1.f / target_fps);

		draw();

		auto frame_duration = std::chrono::steady_clock::now() - frame_begin;
		if (frame_duration < target_frame_duration)
		{
			auto sleep_time = target_frame_duration - frame_duration;
			auto start = std::chrono::steady_clock::now();
			while (std::chrono::steady_clock::now() - start < sleep_time);
		}
	}

    return 0;
}