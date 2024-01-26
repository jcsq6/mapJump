#include "game.h"

const gl_controlled_data<shader> &texture_program()
{
	static constexpr const char *vertex_shader = 
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

	static constexpr const char *fragment_shader = 
		"#version 330 core\n" // fragment shader
		"uniform sampler2D text;"
		"in vec2 texture_coord;"
		"out vec4 frag_color;"
		"void main(){\n"
		"	frag_color = texture(text, texture_coord);"
		"}";

	static auto &program = construct_and_attach<shader>(vertex_shader, fragment_shader);
	return program;
}

static const glm::vec2 square_pts[] = {{-.5f, -.5f}, {.5f, -.5f}, {.5f, .5f}, {-.5f, .5f}};
static const glm::vec2 triangle_pts[] = {{-.5f, -.5f}, {.5f, -.5f}, {0, .5f}};

shapes::shape_buffers::shape_buffers()
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

shapes::shapes() : square{std::begin(square_pts), std::end(square_pts)}, triangle{std::begin(triangle_pts), std::end(triangle_pts)}, buffers{construct_and_attach<shapes::shape_buffers>()}
{
}

game::player_data::player_data() :
	poly{shapes::instance().square, {}, {game::player_size, game::player_size}, 0},
	vel{0, 0}, accel{0, game::gravity}, angle_vel{0}, angle{0},
	on_ground{false}, stopping_left{}, stopping_right{}, x_dir{0},
	on_wall{0}, do_jump{false},
	intangible{true}
{
}

game::game(int target_width, int target_height, std::vector<level> &&_levels) : target_scale{target_width, target_height}, levels{std::move(_levels)}
{
	if (levels.empty())
	{
		level new_level;
		new_level.construct_default();
		levels.push_back(std::move(new_level));
	}

	load_level(0);
}

// assumes ortho has been set and text has been set
void game::draw() const
{
	const auto &assets = game_assets::instance().get();
	const auto &program = texture_program().get();
	const auto &_shapes = shapes::instance();

	// print background
	auto m = glm::scale(glm::translate(glm::mat4(1.f), {target_scale / 2.f, 0}), {target_scale, 0});
	glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);

	glBindTexture(GL_TEXTURE_2D, assets.background.id);

	glBindVertexArray(_shapes.square_vao().id);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	levels[cur_level].draw(is_blue ? color::blue : color::red);

	m = model(player.poly.offset, player.poly.scale, player.angle);
	glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);

	glBindTexture(GL_TEXTURE_2D, assets.player_text.id);

	glBindVertexArray(_shapes.square_vao().id);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

#ifdef MAPJUMP_DEBUG
#include <iostream>
#endif

void game::update(float dt)
{
	static constexpr float air_accel_divisor = 3;
	float stopping_accel = 1800;
	float starting_accel = 600;

	if (!player.on_ground)
	{
		stopping_accel /= air_accel_divisor;
		starting_accel /= air_accel_divisor;
	}

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
			// if was moving right, come to a quick halt before starting moving left
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

	// if the player is stopping from moving left and has come to a halt
	if (player.stopping_left && player.vel.x >= 0)
	{
		player.vel.x = 0;
		player.accel.x = 0;
		player.stopping_left = false;
	}
	// if the player is stopping from moving right and has come to a halt
	if (player.stopping_right && player.vel.x <= 0)
	{
		player.vel.x = 0;
		player.accel.x = 0;
		player.stopping_right = false;
	}

	static constexpr float max_x_vel = 300;

	if (player.vel.x < -max_x_vel)
	{
		player.accel.x = 0;
		player.vel.x = -max_x_vel;
	}
	else if (player.vel.x > max_x_vel)
	{
		player.accel.x = 0;
		player.vel.x = max_x_vel;
	}

	player.poly.offset += player.vel * dt;

	player.angle += player.angle_vel * dt;

	player.x_dir = 0;

	collisions.clear();

	// flag set if the player only collided with neutral blocks
	bool clear_intangible = true;

	// initial pass to resolve collisions
	for (const auto &b : levels[cur_level].blocks)
	{
		if (!is_on(b.block_color))
			continue;
		
		if (collision c = collides(player.poly, b.poly))
		{
			if (b.block_type == block::type::spike)
			{
				reset_level();
				return;
			}

			// if it's collided with a colored block, then don't make tangible
			if (b.block_color != color::neutral)
				clear_intangible = false;

			if (!player.intangible || b.block_color == color::neutral)
			{
				player.poly.offset += c.mtv;
				collisions.push_back(&b);
			}
		}
	}

	if (clear_intangible)
		player.intangible = false;

	static constexpr float epsilon = .1f;

	bool was_on_ground = player.on_ground;
	player.on_ground = false;
	player.on_wall = 0;
	// do a second pass to determine if the player is on any walls and which ones
	// can't be done using mtv of collision resolution
	for (const block *b : collisions)
	{
		glm::vec2 block_min = b->poly.offset - b->poly.scale / 2.f;
		glm::vec2 block_max = b->poly.offset + b->poly.scale / 2.f;
		glm::vec2 player_min = player.poly.offset - player.poly.scale / 2.f;
		glm::vec2 player_max = player.poly.offset + player.poly.scale / 2.f;

		// if they're touching horizontally (if player right is > block left and block right is > player lefts)
		if (player_max.x > block_min.x && block_max.x > player_min.x)
		{
			// if bottom of player is touching top of block
			if (std::abs(player_min.y - block_max.y) < epsilon)
			{
				player.on_ground = true;
				player.vel.y = 0;
			}
			// if top of player is touching bottom of block
			else if (std::abs(player_max.y - block_min.y) < epsilon)
			{
				player.vel.y = 0;
			}
		}

		// if they're touching vertically (if the player top is > block bottom and block top is > player bottom)
		if (player_max.y > player_min.y && block_max.y > player_min.y)
		{
			// if left of player is touching right of block
			if (std::abs(player_min.x - block_max.x) < epsilon)
			{
				player.vel.x = 0;
				if (b->block_type == block::type::jump)
					player.on_wall = -1;
			}
			// if right of player is touching left of block
			else if (std::abs(player_max.x - block_min.x) < epsilon)
			{
				player.vel.x = 0;
				if (b->block_type == block::type::jump)
					player.on_wall = 1;
			}
		}
	}

	// if back on ground, change acceleration
	if (!was_on_ground && player.on_ground)
		player.accel.x *= air_accel_divisor;
	
	// if it's gone 90 degrees
	if (std::abs(player.angle) >= glm::pi<float>() / 2)
	{
		if (player.on_ground)
		{
			player.angle_vel = 0;
			player.angle = 0;
		}
		else
			player.angle -= glm::pi<float>() / 2;
	}
	
	static constexpr float jump_velocity = 500;
	static constexpr float wall_jump_velocity = 300;
	static constexpr float jump_angular_velocity = 2 * glm::pi<float>();

	if (player.do_jump && (std::chrono::high_resolution_clock::now() - player.jump_start) < std::chrono::milliseconds(300))
	{
		if (player.on_ground || player.on_wall)
		{
			player.vel.y = jump_velocity;
			if (player.vel.x > 0)
				player.angle_vel = -jump_angular_velocity;
			else
				player.angle_vel = jump_angular_velocity;
			player.do_jump = false;

			if (!player.on_ground)
			{
				if (player.on_wall > 0)
				{
					player.vel.x = -wall_jump_velocity;
					player.stopping_right = false;
				}
				else
				{
					player.vel.x = wall_jump_velocity;
					player.stopping_left = false;
				}
			}
		}
	}
}

void game::switch_colors()
{
	is_blue = !is_blue;
	// pass to check for player stuck in block
	for (const auto &b : levels[cur_level].blocks)
		if (is_blue == (b.block_color == color::blue) && collides(player.poly, b.poly))
			player.intangible = true;
}

void game::load_level(std::size_t level)
{
	auto &l = levels[level];
	cur_level = level;
	player.poly.offset = (glm::vec2(l.start) + glm::vec2(.5, .5)) * glm::vec2(block_size, block_size);
	is_blue = l.blue_starts;
	collisions.reserve(l.blocks.size());
}

void game::reset_level()
{
	auto &l = levels[cur_level];
	player.vel = {0, 0};
	player.accel.x = 0;
	player.do_jump = false;
	player.intangible = true;
	player.on_wall = 0;
	player.on_ground = false;
	player.stopping_left = player.stopping_right = false;
	player.x_dir = 0;
	player.poly.offset = (glm::vec2(l.start) + glm::vec2(.5, .5)) * glm::vec2(block_size, block_size);
	player.angle = 0;
}