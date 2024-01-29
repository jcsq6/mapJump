#include "level.h"
#include "game.h"

#include <fstream>
#include <stdexcept>

#include <regex>

#include <map>

block::block(glm::ivec2 grid_loc, type _block_type, color _block_color, direction dir) : block_type{_block_type}, block_color{_block_color}
{	
	if (_block_type == block::type::spike)
	{
		poly.scale = {game::block_size, game::block_size / 2};
		poly.poly = &triangle();
	}
	else
	{
		poly.scale = {game::block_size, game::block_size};
		poly.poly = &square();
	}

	if (_block_type == block::type::spawn_anchor || _block_type == block::type::end_anchor)
	{
		block_color = color::no_color;
		dir = direction::up;
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

void block::draw(color active_color, const gl_instance &gl, float transparency) const
{
	const auto &assets = gl.get_assets();

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
				text = &assets.blue_jump;
			else
				text = &assets.blue_cube_fade;
			break;
		case color::red:
			if (on)
				text = &assets.red_jump;
			else
				text = &assets.red_cube_fade;
			break;
		case color::neutral: // for readability
		default: // if no_color is somehow here, just do neutral
			text = &assets.neutral_jump;
			break;
		}
		buff = &gl.get_shapes().square_vao();
		break;
	case block::type::spike:
		switch (block_color)
		{
		case color::blue:
			if (on)
				text = &assets.blue_spike;
			else
				text = &assets.blue_spike_fade;
			break;
		case color::red:
			if (on)
				text = &assets.red_spike;
			else
				text = &assets.red_spike_fade;
			break;
		case color::neutral: // for readability
		default: // if no_color is somehow here, just do neutral
			text = &assets.neutral_spike;
			break;
		}
		buff = &gl.get_shapes().triangle_vao();
		break;
	case block::type::normal:
		switch (block_color)
		{
		case color::blue:
			if (on)
				text = &assets.blue_cube;
			else
				text = &assets.blue_cube_fade;
			break;
		case color::red:
			if (on)
				text = &assets.red_cube;
			else
				text = &assets.red_cube_fade;
			break;
		case color::neutral: // for readability
		default: // if no_color is somehow here, just do neutral
			text = &assets.neutral_cube;
			break;
		}
		buff = &gl.get_shapes().square_vao();
		break;
	case block::type::spawn_anchor:
		text = &assets.spawn_anchor;
		buff = &gl.get_shapes().square_vao();
		break;
	case block::type::end_anchor:
		text = &assets.end_anchor;
		buff = &gl.get_shapes().square_vao();
		break;
	}

	auto m = model(poly.offset, poly.scale, poly.angle);
	glUniformMatrix4fv(glGetUniformLocation(gl.get_texture_program().id, "model"), 1, GL_FALSE, &m[0][0]);
	glUniform1f(glGetUniformLocation(gl.get_texture_program().id, "transparency"), transparency);

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
	start = glm::ivec2{1, 1};
	end = glm::ivec2{game::map_width - 2, 1};
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

void level::draw(color active_color, const gl_instance &gl) const
{
	for (const auto &b : blocks)
		b.draw(active_color, gl);
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

		if (temp == start || temp == end)
			continue;

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

std::vector<level> get_levels(const std::filesystem::path &location)
{
	std::vector<level> levels;

	auto status = std::filesystem::status(location);
	// read singular level
	if (status.type() == std::filesystem::file_type::regular)
	{
		level new_level;
		try
		{
			new_level.read_level(location);
		}
		catch(const std::exception&)
		{
			// std::cout << "Couldn't read level " << location << ": " << e.what() << '\n';
			new_level.construct_default();
		}
		levels.push_back(std::move(new_level));
	}
	// read multiple levels in order _n
	else if (status.type() == std::filesystem::file_type::directory)
	{
		static std::regex name_pattern("\\S*_(\\d*).lvl");
		std::map<unsigned int, level> level_map;
		for (const auto &entry : std::filesystem::directory_iterator{location})
		{
			if (!entry.is_regular_file())
				continue;

			auto path = entry.path();
			auto path_str = path.string();

			unsigned int index;
			if (std::smatch match; std::regex_match(path_str, match, name_pattern))
			{
				try
				{
					index = std::stoul(match[1].str());
				}
				catch(...)
				{
					// std::cout << '"' << path << "\": Incorrect file name format\n";
					continue;
				}
				
			}
			else
			{
				// std::cout << '"' << path << "\": Incorrect file name format\n";
				continue;
			}

			level new_level;
			try
			{
				new_level.read_level(path);
			}
			catch(const std::exception&)
			{
				// std::cout << "Couldn't read level " << location << ": " << e.what() << '\n';
				continue;
			}

			level_map.emplace(index, std::move(new_level));
		}

		if (level_map.empty())
		{
			level new_level;
			new_level.construct_default();
			levels.push_back(std::move(new_level));
		}
		else
		{
			levels.reserve(level_map.size());
			for (auto &lvl : level_map)
				levels.push_back(std::move(lvl.second));
		}
	}

	return levels;
}
