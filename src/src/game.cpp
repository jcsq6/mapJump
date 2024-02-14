#include "game.h"

game::player_data::player_data() :
	poly{square(), {}, {game::player_size, game::player_size}, 0},
	vel{0, 0}, accel{0, game::gravity}, angle_vel{0}, angle{0},
	on_ground{false}, stopping_left{}, stopping_right{}, x_dir{0},
	on_wall{0}, do_jump{false},
	intangible{true}
{
}

// assumes ortho has been set and text has been set
void game::draw(const gl_instance &gl) const
{
	const auto &assets = gl.get_assets();
	const auto &program = gl.get_texture_program();
	const auto &_shapes = gl.get_shapes();

	print_background(gl);

	levels[cur_level].draw(is_blue ? color::blue : color::red, gl);

	auto m = model(player.poly.offset, player.poly.scale, player.angle);
	glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &m[0][0]);

	glBindTexture(GL_TEXTURE_2D, assets.player_text.id);

	glBindVertexArray(_shapes.square_vao().id);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

#ifdef MAPJUMP_DEBUG
#include <iostream>
#endif

bool same_dir(const glm::vec2 &a, const glm::vec2 &b)
{
	if (b.x == 0)
		return (a.y >= 0) == (b.y >= 0); // if they're both positive 
	else if (b.y == 0)
		return (a.x >= 0) == (b.x >= 0); // if they're both positive
	
	float xscale = a.x / b.x;
	float yscale = a.y / b.y;
	return xscale == yscale;
}

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
			// if it's collided with a colored block, then don't make tangible
			if (b.block_color != color::neutral)
				clear_intangible = false;

			if (!player.intangible || b.block_color == color::neutral)
			{
				player.poly.offset += c.mtv;

				collisions.push_back({&b, c});
			}
		}
	}

	if (clear_intangible)
		player.intangible = false;

	static constexpr float epsilon = .05f;

	bool was_on_ground = player.on_ground;
	player.on_ground = false;
	player.on_wall = 0;
	// do a second pass to determine if the player is on any walls and which ones
	// can't be done using mtv of collision resolution

	glm::vec2 player_min = player.poly.offset - player.poly.scale / 2.f;
	glm::vec2 player_max = player.poly.offset + player.poly.scale / 2.f;
	for (const auto [b, c] : collisions)
	{

		// if (b->block_type == block::type::spike)
		// {
		// 	glm::vec2 flat_normal = b->poly.normal(0);
		// 	glm::vec2 diff = b->poly.center() - player.poly.center();

		// 	float flat_normal_proj = glm::dot(flat_normal, diff);
		// 	float norm1_proj = glm::dot(b->poly.normal(1), diff);
		// 	float norm2_proj = glm::dot(b->poly.normal(2), diff);

		// 	if (flat_normal.y != 0)
		// 	{
		// 		player.vel.y = 0;
		// 		// if the edge normal is pointed up, it's on ground
		// 		if (flat_normal.y > 0)
		// 			player.on_ground = true;
		// 	}
		// 	else
		// 		player.vel.x = 0;
		// }
		// else
		{
			bool is_spike = b->block_type == block::type::spike;
			glm::vec2 block_min, block_max;
			if (is_spike)
			{
				block_min = b->poly.transform({-.5, -.5});
				block_max = b->poly.transform({.5, .5});

				if (block_min.x > block_max.x)
					std::swap(block_min.x, block_max.x);
				if (block_min.y > block_max.y)
					std::swap(block_min.y, block_max.y);
			}
			else
			{
				block_min = b->poly.offset - b->poly.scale / 2.f;
				block_max = b->poly.offset + b->poly.scale / 2.f;
			}

			// touching horizontally (if player right is > block left and block right is > player lefts)
			bool touching_horizontally = player_max.x - block_min.x > epsilon && block_max.x - player_min.x > epsilon;
			// if they're touching vertically (if the player top is > block bottom and block top is > player bottom)
			bool touching_vertically = player_max.y - block_min.y > epsilon && block_max.y - player_min.y > epsilon;

			if (touching_horizontally)
			{
				if (is_spike)
				{
					direction d = b->dir();
					// if it's not touching the flat side of the spike
					if ((d == direction::up || d == direction::down) && !same_dir(-c.normal, b->poly.normal(0)))
					{
						reset_level();
						return;
					}
				}

				// if bottom of player is touching top of block
				if (std::abs(player_min.y - block_max.y) < epsilon)
				{
					player.on_ground = true;
					player.vel.y = 0;
				}
				// if top of player is touching bottom of block
				else if (std::abs(player_max.y - block_min.y) < epsilon)
					player.vel.y = 0;
			}

			if (touching_vertically)
			{
				if (is_spike)
				{
					direction d = b->dir();
					// if it's not touching the flat side of the spike
					if ((d == direction::left || d == direction::right) && !same_dir(-c.normal, b->poly.normal(0)))
					{
						reset_level();
						return;
					}
				}

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
	
	static constexpr float jump_velocity = 550;
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

	if (glm::ivec2(player.poly.offset / (float)game::block_size) == levels[cur_level].end)
	{
		if (cur_level != levels.size() - 1)
			load_level(cur_level + 1);
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