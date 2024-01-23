#ifndef GAME_H
#define GAME_H

#include "game_assets.h"
#include "collision.h"

#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <list>

struct shapes
{
	polygon square;
	polygon triangle;
	vao square_vao;
	vao triangle_vao;

	static constexpr int pos_attribute = 0;
	static constexpr int text_pos_attribute = 1;

	static const shapes &instance()
	{
		static const shapes s;
		return s;
	}

private:
	vbo square_vbo;
	vbo triangle_vbo;
	shapes();
};

struct block
{
	enum class type
	{
		normal,
		jump,
		spike,
	};
	enum class color
	{
		blue,
		red,
		neutral,
	};

	polygon_view poly;
	type block_type;
	color block_color;
};

struct level
{
	std::vector<block> blocks;
	glm::vec2 start;
	glm::vec2 end;
};

class game
{
public:
	game(int window_width, int window_height);

	void draw() const;
	void update(float dt);

	void move_right() { ++player.x_dir; }
	void move_left() { --player.x_dir; }
	void jump()
	{
		player.do_jump = true;
		player.jump_start = std::chrono::high_resolution_clock::now();
	}

private:
	game_assets assets;
	shader program;

	std::vector<level> levels;
	std::size_t cur_level;

	struct player_data
	{
		player_data();

		polygon_view poly;
		glm::vec2 vel;
		glm::vec2 accel;
		bool on_ground;
		bool stopping_right; // if you're moving right but slowing down
		bool stopping_left; // if you're moving left but slowing down
		int x_dir; // > 0 for right, < 0 for left, 0 for still (reset to 0 after each update)
		bool do_jump; // set to true when game::jump is called, false after jump initiated
		std::chrono::high_resolution_clock::time_point jump_start; // time when jump was requested
	};

	player_data player;

	glm::mat4 ortho;

	bool is_blue;

	bool is_on(const block &b) const
	{
		return b.block_color == block::color::neutral || (b.block_color == block::color::blue) == is_blue;
	}

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
};

#endif