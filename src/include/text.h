#ifndef TEXT_H
#define TEXT_H
#include "gl_object.h"
#include <unordered_map>
#include <glm/mat4x4.hpp>

#include "rect.h"

class gl_instance;
struct FT_FaceRec_;
struct library_handle;

class font
{
public:
	static constexpr unsigned int default_height = 48;

	font() = default;

	font(const void *data, std::size_t size, unsigned int height) : font()
	{
		load(data, size, height);
	}

	void load(const void *data, std::size_t size, unsigned int height);

	unsigned int get_character_height() const { return face.size; }

private:
	friend class text;

	struct face_handle
	{
		FT_FaceRec_ *face;
		unsigned int size;

		face_handle();
		~face_handle();

		void load(library_handle &lib, const void *data, std::size_t size);

		face_handle(face_handle &&other);
		face_handle &operator=(face_handle &&other);

		void resize();
	};

	face_handle face;

	struct character
	{
		character() : text{}, offset{}, advance{}, height{} {}

		character(const font *_font, uint32_t c);
		~character() = default;

		// dummy functions to enable use with std::map
		character(const character &other);
		character &operator=(const character &other);

		character(character &&other);
		character &operator=(character &&other);

		void load(const font *_font, uint32_t c);

		texture text;

		glm::ivec2 offset;
		unsigned int advance;
		unsigned int height;
	};

	character const *at(uint32_t c) const
	{
		if (!m_chars.count(c))
			return &m_chars.emplace(c, character{this, c}).first->second;
		return &m_chars.at(c);
	}

	// mutable to allow potential addition of new characters in draw function
	mutable std::unordered_map<uint32_t, character> m_chars;
};

class text
{
public:
	text() : m_origin{},
			 m_scale{1, 1},
			 m_font{},
			 m_data{}
	{
	}
	text(font &_font) : m_origin{},
						m_scale{1, 1},
						m_font{&_font},
						m_data{}
	{
	}
	text(std::basic_string_view<char> txt, font &_font) : m_origin{},
														  m_scale{1, 1},
														  m_font{&_font},
														  m_data{txt.begin(), txt.end()}
	{
	}

	void set_string(std::basic_string_view<char> txt) { m_data.assign(txt.begin(), txt.end()); }

	const std::basic_string<uint32_t> &get_string() const { return m_data; }

	void set_text_origin(glm::vec2 origin) { m_origin = origin; }
	glm::vec2 get_text_origin() const { return m_origin; }

	void set_text_scale(glm::vec2 scale) { m_scale = scale; }
	glm::vec2 get_text_scale() const { return m_scale; }

	void set_font(font& _font) { m_font = &_font; }
	font const* get_font() const { return m_font;  }

	rect get_local_rect() const;

	void draw(gl_instance &gl) const;

private:
	std::basic_string<uint32_t> m_data;
	glm::vec2 m_origin;
	glm::vec2 m_scale;
	font* m_font;
};
#endif