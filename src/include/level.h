#ifndef LEVEL_H
#define LEVEL_H

#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <filesystem>

#include "collision.h"

enum class color : char
{
	blue,
	red,
	neutral,
	no_color,
};

enum direction : char
{
	up,
	right,
	down,
	left,
};

struct block
{
	enum class type : char
	{
		normal,
		jump,
		spike,
		spawn_anchor,
		end_anchor,
	};

	// dir is direction the block is facing
	block(glm::ivec2 grid_loc, type _block_type, color _block_color, direction dir);

	// assumes ortho has been set and text has been set
	void draw(color active_color) const;

	direction dir() const;

	polygon_view poly;
	type block_type;
	color block_color;
};

struct level
{
	std::vector<block> blocks;
	glm::ivec2 start;
	glm::ivec2 end;
	bool blue_starts;

	void construct_default();
	// assumes ortho has been set and text has been set
	void draw(color active_color) const;

	void read_level(const std::filesystem::path &filename);
	void write_level(const std::filesystem::path &filename);
};

// pass in directory to level_location to load multiple levels, or a single file to load one level
// the levels in a directory must be of the format "levelName_*levelNumber*.lvl". level numbers are sorted by *levelNumber*
std::vector<level> get_levels(const std::filesystem::path &location);
#endif