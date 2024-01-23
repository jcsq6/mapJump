#include "game.h"

shapes::shapes() : square{{glm::vec2{0, 0}, {1, 0}, {1, 1}, {0, 1}}}, triangle{{glm::vec2{0, 0}, {1, 0}, {.5, .5}}}
{
	// buffer square data
	glBindBuffer(GL_ARRAY_BUFFER, square_vbo.id);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), &(*square.begin()), GL_STATIC_DRAW);

	// setup square vao
	glBindVertexArray(square_vao.id);

	glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(pos_attribute);
	glVertexAttribPointer(text_pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(text_pos_attribute);

	// buffer triangle data
	glBindBuffer(GL_ARRAY_BUFFER, triangle_vbo.id);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2), &(*triangle.begin()), GL_STATIC_DRAW);

	glBindVertexArray(triangle_vao.id);

	// setup triangle vao
	glVertexAttribPointer(pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(pos_attribute);
	glVertexAttribPointer(text_pos_attribute, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(text_pos_attribute);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

game::player_data::player_data() : poly{shapes::instance().square, {}, {40, 40}, 0}, vel{0, 0}, accel{0, -1200}, on_ground{false}, stopping_left{}, stopping_right{}, x_dir{0}, do_jump{false}
{
}

game::game(int window_width, int window_height) : program(vertex_shader, fragment_shader),
												  cur_level{0},
												  ortho{glm::ortho<float>(0, (float)window_width, 0, (float)window_height, -1, 1)},
												  is_blue{true}
{
	player.poly.offset = {64, 64}; // here for future reading of initial level
	levels.push_back({});
	auto &l = levels.back();
	for (int i = 0; i < 15; ++i)
	{
		l.blocks.push_back({{shapes::instance().square, {i * 64, 0}, {64, 64}, 0}, block::type::normal, block::color::neutral});
		l.blocks.push_back({{shapes::instance().square, {i * 64, 8 * 64}, {64, 64}, 0}, block::type::normal, block::color::neutral});
	}

	for (int i = 1; i < 8; ++i)
	{
		l.blocks.push_back({{shapes::instance().square, {0, i * 64}, {64, 64}, 0}, block::type::normal, block::color::neutral});
		l.blocks.push_back({{shapes::instance().square, {14 * 64, i * 64}, {64, 64}, 0}, block::type::normal, block::color::neutral});
	}
}

void game::draw() const
{
	glUseProgram(program.id);
	glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &ortho[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program.id, "text"), 0);

	const level &cur = levels[cur_level];

	for (const auto &b : cur.blocks)
	{
		if (!is_on(b))
			continue;
		
		const texture *text;
		const vao *buff;
		switch (b.block_type)
		{
		case block::type::jump:
			switch (b.block_color)
			{
			case block::color::blue:
				text = &assets.neutral_jump;
				break;
			case block::color::red:
				text = &assets.red_jump;
				break;
			case block::color::neutral:
				text = &assets.neutral_jump;
				break;
			}
			buff = &shapes::instance().square_vao;
			break;
		case block::type::spike:
			switch (b.block_color)
			{
			case block::color::blue:
				text = &assets.neutral_spike;
				break;
			case block::color::red:
				text = &assets.red_spike;
				break;
			case block::color::neutral:
				text = &assets.neutral_spike;
				break;
			}
			buff = &shapes::instance().triangle_vao;
			break;
		case block::type::normal:
			switch (b.block_color)
			{
			case block::color::blue:
				text = &assets.neutral_cube;
				break;
			case block::color::red:
				text = &assets.red_cube;
				break;
			case block::color::neutral:
				text = &assets.neutral_cube;
				break;
			}
			buff = &shapes::instance().square_vao;
			break;
		}

		auto model = b.poly.model();
		glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &model[0][0]);

		glBindTexture(GL_TEXTURE_2D, text->id);

		glBindVertexArray(buff->id);
		glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(b.poly.size()));
	}

	auto model = player.poly.model();
	glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &model[0][0]);

	glBindTexture(GL_TEXTURE_2D, assets.player_text.id);

	glBindVertexArray(shapes::instance().square_vao.id);
	glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(4));
}

#include <iostream>

void game::update(float dt)
{
	static constexpr float stopping_accel = 1800;
	static constexpr float starting_accel = 600;
	// stop moving
	if (player.x_dir == 0)
	{
		if (player.vel.x < 0)
		{
			player.stopping_left = true;
			player.accel.x = stopping_accel;
		}
		else if (player.vel.x > 0)
		{
			player.stopping_right = true;
			player.accel.x = -stopping_accel;
		}
	}
	else
	{
		// move right
		if (player.x_dir > 0)
		{
			// if moving left, come to a quick halt before starting moving right
			if (player.vel.x < 0)
			{
				player.stopping_left = true;
				player.accel.x = stopping_accel;
			}
			else
				player.accel.x = starting_accel;
		}
		// move left
		else
		{
			if (player.vel.x > 0)
			{
				player.stopping_right = true;
				player.accel.x = -stopping_accel;
			}
			else
				player.accel.x = -starting_accel;
		}
	}

	player.vel += player.accel * dt;
	// if the player is stopping and has come to a halt
	if (player.stopping_left && player.vel.x >= 0)
	{
		player.vel.x = 0;
		player.accel.x = 0;
		player.stopping_left = false;
	}
	// if the player is stopping and has come to a halt
	if (player.stopping_right && player.vel.x <= 0)
	{
		player.vel.x = 0;
		player.accel.x = 0;
		player.stopping_right = false;
	}

	if (player.vel.x < -300 || player.vel.x > 300)
		player.accel.x = 0;

	player.poly.offset += player.vel * dt;
	player.x_dir = 0;

	player.on_ground = false;
	player.in_air = true;
	for (const auto &b : levels[cur_level].blocks)
	{
		if (!is_on(b))
			continue;
		// if is on screen
		
		collision c = collides(player.poly, b.poly);
		if (!c)
			continue;
		// if collides

		player.poly.offset += c.mtv;
		if (c.mtv.y > 0)
		{
			player.on_ground = true;
			player.vel.y = 0;
		}
		else if (c.mtv.y < 0)
		{
			player.vel.y = 0;
		}

		if (c.mtv.x != 0)
			player.vel.x = 0;
	}

	if (player.do_jump && (std::chrono::high_resolution_clock::now() - player.jump_start) < std::chrono::milliseconds(300))
	{
		if (player.on_ground)
		{
			player.vel.y = 500;
			player.do_jump = false;
		}
	}
}