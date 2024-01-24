#include "level.h"
#include "game.h"

#include <fstream>
#include <stdexcept>

block::block(glm::ivec2 grid_loc, type _block_type, color _block_color, direction dir) : block_type{_block_type}, block_color{_block_color}
{	
	const auto &_shapes = shapes::instance().get();
	if (_block_type == block::type::spike)
	{
		poly.scale = {game::block_size, game::block_size / 2};
		poly.poly = &_shapes.triangle;
	}
	else
	{
		poly.scale = {game::block_size, game::block_size};
		poly.poly = &_shapes.square;
	}

	glm::vec2 poly_trans;

	switch (dir)
	{
	case direction::up:
		poly.angle = 0;
		poly_trans = {poly.scale.x / 2, poly.scale.y / 2};
		break;
	case direction::left:
		poly.angle = glm::pi<float>() / 2;
		poly_trans = {game::block_size - poly.scale.y / 2, poly.scale.x / 2};
		break;
	case direction::down:
		poly.angle = glm::pi<float>();
		poly_trans = {poly.scale.x / 2, game::block_size - poly.scale.y / 2};
		break;
	case direction::right:
		poly.angle = 3 * glm::pi<float>() / 2;
		poly_trans = {poly.scale.y / 2, poly.scale.x / 2};
		break;
	}

	poly.offset = glm::vec2(grid_loc) * (float)game::block_size + poly_trans;
}

void block::draw(color active_color) const
{
	bool on = active_color == color::no_color || active_color == block_color || block_color == color::neutral;
		
	const texture *text;
	const vao *buff;
	switch (block_type)
	{
	case block::type::jump:
		switch (block_color)
		{
		case color::blue:
			if (on)
				text = &game_assets::instance().get().blue_jump;
			else
				text = &game_assets::instance().get().blue_cube_fade;
			break;
		case color::red:
			if (on)
				text = &game_assets::instance().get().red_jump;
			else
				text = &game_assets::instance().get().red_cube_fade;
			break;
		case color::neutral: // for readability
		default: // if no_color is somehow here, just do neutral
			text = &game_assets::instance().get().neutral_jump;
			break;
		}
		buff = &shapes::instance().get().square_vao;
		break;
	case block::type::spike:
		switch (block_color)
		{
		case color::blue:
			if (on)
				text = &game_assets::instance().get().blue_spike;
			else
				text = &game_assets::instance().get().blue_spike_fade;
			break;
		case color::red:
			if (on)
				text = &game_assets::instance().get().red_spike;
			else
				text = &game_assets::instance().get().red_spike_fade;
			break;
		case color::neutral: // for readability
		default: // if no_color is somehow here, just do neutral
			text = &game_assets::instance().get().neutral_spike;
			break;
		}
		buff = &shapes::instance().get().triangle_vao;
		break;
	case block::type::normal:
		switch (block_color)
		{
		case color::blue:
			if (on)
				text = &game_assets::instance().get().blue_cube;
			else
				text = &game_assets::instance().get().blue_cube_fade;
			break;
		case color::red:
			if (on)
				text = &game_assets::instance().get().red_cube;
			else
				text = &game_assets::instance().get().red_cube_fade;
			break;
		case color::neutral: // for readability
		default: // if no_color is somehow here, just do neutral
			text = &game_assets::instance().get().neutral_cube;
			break;
		}
		buff = &shapes::instance().get().square_vao;
		break;
	}

	auto m = model(poly.offset, poly.scale, poly.angle);
	glUniformMatrix4fv(glGetUniformLocation(texture_program().get().id, "model"), 1, GL_FALSE, &m[0][0]);

	glBindTexture(GL_TEXTURE_2D, text->id);

	glBindVertexArray(buff->id);
	glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(poly.size()));
}

direction block::dir() const
{
	if (poly.angle == glm::pi<float>() / 2)
		return direction::left;
	else if (poly.angle == glm::pi<float>())
		return direction::down;
	else if (poly.angle == 3 * glm::pi<float>() / 2)
		return direction::right;
	else // default up, 0 is up as well
		return direction::up;
}

void level::construct_default()
{
	start = {1, 1};
	end = {game::map_width - 1, 1};
	blue_starts = true;

	// top and bottom wall
	for (int i = 0; i < 15; ++i)
	{
		blocks.emplace_back(glm::ivec2{i, 0}, block::type::normal, color::neutral, direction::up);
		blocks.emplace_back(glm::ivec2{i, 8}, block::type::normal, color::neutral, direction::up);
	}

	// left and right wall
	for (int i = 1; i < 8; ++i)
	{
		blocks.emplace_back(glm::ivec2{0, i}, block::type::normal, color::neutral, direction::up);
		blocks.emplace_back(glm::ivec2{14, i}, block::type::normal, color::neutral, direction::up);
	}
}

void level::draw(color active_color) const
{
	for (const auto &b : blocks)
		b.draw(active_color);
}

const std::string header_tag = "MapJumpLevelFile";

using vec_type = glm::vec<2, std::int32_t>;

void level::read_level(const std::filesystem::path &filename)
{
	blocks.clear();
	start = {0, 0};
	end = {0, 0};
	blue_starts = true;

	if (filename.extension() != ".lvl")
		throw std::runtime_error("Can only read .lvl files");

	std::ifstream in(filename, std::ios_base::binary);
	if (!in)
		throw std::runtime_error("Couldn't open file for reading");
	
	{
		std::string header(header_tag.size(), '\0');
		if (!in.read(header.data(), header.size()) || header != header_tag)
			throw std::runtime_error("Invalid file");
	}

	std::uint32_t size;
	if (!in.read(reinterpret_cast<char *>(&size), sizeof(size)))
		throw std::runtime_error("Invalid file");

	vec_type temp;
	if (!in.read(reinterpret_cast<char *>(&temp), sizeof(temp)))
		throw std::runtime_error("Invalid file");
	start = temp;

	if (!in.read(reinterpret_cast<char *>(&temp), sizeof(temp)))
		throw std::runtime_error("Invalid file");
	end = temp;

	if (!in.read(reinterpret_cast<char *>(&blue_starts), sizeof(blue_starts)))
		throw std::runtime_error("Invalid file");

	blocks.clear();
	blocks.reserve(size);
	for (std::uint32_t i = 0; i < size; ++i)
	{
		block::type block_type;		
		if (!in.read(reinterpret_cast<char *>(&block_type), sizeof(block_type)))
			throw std::runtime_error("Invalid file");

		color block_color;
		if (!in.read(reinterpret_cast<char *>(&block_color), sizeof(block_color)))
			throw std::runtime_error("Invalid file");

		direction dir;
		if (!in.read(reinterpret_cast<char *>(&dir), sizeof(dir)))
			throw std::runtime_error("Invalid file");
		
		if (!in.read(reinterpret_cast<char *>(&temp), sizeof(temp)))
			throw std::runtime_error("Invalid file");

		blocks.emplace_back(temp, block_type, block_color, dir);
	}
}

void level::write_level(const std::filesystem::path &filename)
{
	auto path = filename;
	path.replace_extension(".lvl");

	std::ofstream out(path, std::ios_base::binary);
	if (!out)
		throw std::runtime_error("Couldn't open file for writing");

	out << header_tag;

	{
		std::uint32_t size = static_cast<std::uint32_t>(blocks.size());
		out.write(reinterpret_cast<const char *>(&size), sizeof(size));
	}

	vec_type temp;

	temp = start;
	out.write(reinterpret_cast<const char *>(&temp), sizeof(temp));
	
	temp = end;
	out.write(reinterpret_cast<const char *>(&temp), sizeof(temp));

	out.write(reinterpret_cast<const char *>(&blue_starts), sizeof(blue_starts));

	for (const auto &b : blocks)
	{
		out.write(reinterpret_cast<const char *>(&b.block_type), sizeof(b.block_type));
		out.write(reinterpret_cast<const char *>(&b.block_color), sizeof(b.block_color));

		direction dir = b.dir();
		out.write(reinterpret_cast<const char *>(&dir), sizeof(dir));

		temp = b.poly.offset / glm::vec2(game::block_size, game::block_size);
		out.write(reinterpret_cast<const char *>(&temp), sizeof(temp));
	}
}