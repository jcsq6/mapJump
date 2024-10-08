#ifndef GAME_H
#define GAME_H

#include "game_assets.h"
#include "collision.h"
#include "level.h"

#include "gl_instance.h"

#include <chrono>
#include <filesystem>

#include <ranges>

class game
{
public:
	static constexpr float gravity = -1200;
	static constexpr int block_size = 60;
	static constexpr int player_size = 40;
	static constexpr int map_width = 16;
	static constexpr int map_height = 9;

	template <std::ranges::range LevelRange>
	game(const LevelRange &_levels);

	// assumes ortho has been set and text has been set
	void draw(const gl_instance &gl) const;
	void update(float dt);

	void move_right() { ++player.x_dir; }
	void move_left() { --player.x_dir; }
	void jump()
	{
		player.do_jump = true;
		player.jump_start = std::chrono::high_resolution_clock::now();
	}

	void switch_colors();

	std::size_t current_level() const { return cur_level; }

private:
	void load_level(std::size_t level);
	void reset_level();

	bool is_on(color c) const
	{
		return c == color::neutral || (c == color::blue) == is_blue;
	}
	
	std::vector<level> levels;
	std::size_t cur_level;

	std::vector<std::pair<const block *, collision>> collisions;

	struct player_data
	{
		player_data();

		polygon_view poly;
		glm::vec2 vel;
		glm::vec2 accel;
		float angle_vel;
		float angle; // poly.angle is 0, this angle is what is drawn
		bool on_ground;
		bool stopping_right; // if you're moving right but slowing down
		bool stopping_left; // if you're moving left but slowing down
		int x_dir; // > 0 for right, < 0 for left, 0 for still (reset to 0 after each update)
		int on_wall; // > 0 for on right wall jump, < 0 for on left wall jump, 0 for not on wall
		bool do_jump; // set to true when game::jump is called, false after jump initiated
		std::chrono::high_resolution_clock::time_point jump_start; // time when jump was requested
		bool intangible;
	};

	player_data player;

	bool is_blue;
};

template <std::ranges::range LevelRange>
game::game(const LevelRange &_levels) : levels{std::ranges::begin(_levels), std::ranges::end(_levels)}
{
	if (levels.empty())
	{
		level new_level;
		new_level.construct_default();
		levels.push_back(std::move(new_level));
	}

	load_level(0);
}

#endif