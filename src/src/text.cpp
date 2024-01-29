#include "text.h"
#include "gl_instance.h"

#include <stdexcept>
#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

struct library_handle
{
	FT_Library library;

	library_handle() : library{}
	{
		if (FT_Init_FreeType(&library))
			throw std::runtime_error("Could not initialize freetype.");
	}
	~library_handle()
	{
		FT_Done_FreeType(library);
	}
};

library_handle &get_library()
{
	static library_handle lib;
	return lib;
}

font::face_handle::face_handle() : face{}, size{default_height} {}

void font::face_handle::load(library_handle &lib, const void *data, std::size_t size)
{
	if (FT_New_Memory_Face(lib.library, reinterpret_cast<const FT_Byte *>(data), static_cast<FT_Long>(size), 0, &face))
		throw std::runtime_error("Could not load font.");
}

font::face_handle::~face_handle()
{
	FT_Done_Face(face);
}

font::face_handle::face_handle(face_handle &&other) : face{other.face}, size{other.size}
{
	other.face = nullptr;
	other.size = default_height;
}
font::face_handle &font::face_handle::operator=(face_handle &&other)
{
	FT_Done_Face(face);
	face = other.face;
	size = other.size;
	other.face = nullptr;
	other.size = default_height;

	return *this;
}

void font::face_handle::resize()
{
	FT_Set_Pixel_Sizes(face, 0, size);
}

void font::load(const void *data, std::size_t size, unsigned int height)
{
	m_chars.clear();

	face.load(get_library(), data, size);
	
	face.size = height;
	face.resize();
}

font::character::character(const font *_font, uint32_t c) : text{}, offset{}, advance{}, height{}
{
	load(_font, c);
}

font::character::character(character &&other) : text{std::move(other.text)}, offset{other.offset}, advance{other.advance}, height{other.height}
{
	other.offset = {};
	other.height = {};
	other.advance = {};
}

font::character &font::character::operator=(font::character &&other)
{
	text.~texture();

	text = std::move(other.text);
	offset = other.offset;
	advance = other.advance;
	height = other.height;

	other.offset = {};
	other.height = {};
	other.advance = {};

	return *this;
}

void font::character::load(const font *_font, uint32_t c)
{
	height = _font->get_character_height();

	FT_Face face = _font->face.face;
	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		throw std::runtime_error("Couldn't load character");
	
	text = texture(GL_RGBA, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 1, true);
	offset.x = face->glyph->bitmap_left;
	offset.y = face->glyph->bitmap_top;
	advance = face->glyph->advance.x;
}

font::character::character(const font::character &other)
{
	throw std::runtime_error("You shouldn't be able to get here... For some reason the copy constructor was called for character");
}
font::character &font::character::operator=(const font::character &other)
{
	throw std::runtime_error("You shouldn't be able to get here... For some reason the copy assignment was called for character");
}

rect text::get_local_rect() const
{
	rect res{{0, 0}, {0, 0}};

	if (m_data.empty())
		return res;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glm::vec2 max{-m_font->at(m_data.front())->offset.x, 0};

	auto end = m_data.end() - 1;

	font::character const *cur;

	for (auto it = m_data.begin(); it != end; ++it)
	{
		cur = m_font->at(*it);
		max.x += cur->advance >> 6;

		if (float pot = (float)cur->offset.y - cur->text.height; pot < res.min.y)
			res.min.y = pot;
		if (cur->offset.y > max.y)
			max.y = (float)cur->offset.y;
	}

	cur = m_font->at(*end);
	max.x += cur->offset.x + cur->text.width;

	if (float pot = (float)cur->offset.y - cur->text.height; pot < res.min.y)
		res.min.y = pot;
	if (cur->offset.y > max.y)
		max.y = (float)cur->offset.y;

	res.dims = max - res.min;

	res.dims *= m_scale;
	res.min *= m_scale;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	return res;
}

void text::draw(gl_instance &gl) const
{
	if (m_data.empty())
		return;

	const auto &program = gl.get_text_program();

	glBindVertexArray(gl.get_shapes().square_vao().id);

	// set uniforms
	glUseProgram(program.id);
	glUniformMatrix4fv(glGetUniformLocation(program.id, "ortho"), 1, GL_FALSE, &gl.get_ortho()[0][0]);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(program.id, "text"), 0);

	glUniform4f(glGetUniformLocation(program.id, "color"), 0, 0, 0, 1);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto cur = m_font->at(m_data.front());
	glm::vec2 origin{m_origin.x - cur->offset.x * m_scale.x, m_origin.y};

	for (auto c : m_data)
	{
		cur = m_font->at(c);
		glm::vec2 sz{cur->text.width, cur->text.height};
		glm::vec2 cur_loc = {origin.x + (cur->offset.x * m_scale.x), origin.y + (cur->offset.y - sz.y) * m_scale.y};
		origin.x += (cur->advance >> 6) * m_scale.x;

		glm::vec2 scale = m_scale * sz;

		// cur_loc + scale / 2.f because square vao is centered at origin
		auto model = glm::scale(glm::translate(glm::mat4(1.f), {cur_loc + scale / 2.f, 0}), {scale, 0});
		glUniformMatrix4fv(glGetUniformLocation(program.id, "model"), 1, GL_FALSE, &model[0][0]);

		glBindTexture(GL_TEXTURE_2D, cur->text.id);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}