#ifndef GL_OBJECT_H
#define GL_OBJECT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>

struct window
{
	window() : handle{} {}

	window(int width, int height, const char *title)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		handle = glfwCreateWindow(width, height, title, nullptr, nullptr);
		glfwMakeContextCurrent(handle);

		glewInit();
	}

	window(const window &) = delete;

	window(window &&other) : handle{other.handle} {  other.handle = nullptr; }
	window &operator=(window &&other)
	{
		glfwDestroyWindow(handle);
		handle = other.handle;
		other.handle = nullptr;
		return *this;
	}

	~window()
	{
		glfwDestroyWindow(handle);
		handle = nullptr;
	}

	GLFWwindow *handle;
};

struct shader
{
	shader() : id{} {}
	shader(const char *vertex, const char *fragment) : id{}
	{
		GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_shader_id, 1, &vertex, nullptr);
		glCompileShader(vert_shader_id);

		GLint completed = 0;
		glGetShaderiv(vert_shader_id, GL_COMPILE_STATUS, &completed);
		if (completed == GL_FALSE)
		{
			std::string message;
			int length = 0;
			glGetShaderiv(vert_shader_id, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				message.resize(length);
				int stored_length;
				glGetShaderInfoLog(vert_shader_id, length, &stored_length, message.data());
			}

			std::cerr << "vertex shader compilation failed\n"
					  << message << '\n';
		}

		GLuint frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_shader_id, 1, &fragment, nullptr);
		glCompileShader(frag_shader_id);

		completed = 0;
		glGetShaderiv(frag_shader_id, GL_COMPILE_STATUS, &completed);
		if (completed == GL_FALSE)
		{
			std::string message;
			int length = 0;
			glGetShaderiv(frag_shader_id, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				message.resize(length);
				int stored_length;
				glGetShaderInfoLog(frag_shader_id, length, &stored_length, message.data());
			}

			std::cerr << "fragment shader compilation failed\n"
					  << message << '\n';
		}

		id = glCreateProgram();
		glAttachShader(id, vert_shader_id);
		glAttachShader(id, frag_shader_id);
		glLinkProgram(id);

		glDeleteShader(frag_shader_id);
		glDeleteShader(vert_shader_id);

		completed = 0;
		glGetProgramiv(id, GL_LINK_STATUS, &completed);
		if (completed == GL_FALSE)
		{
			std::string message;
			int length = 0;
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
			if (length)
			{
				message.resize(length);
				int stored_length;
				glGetProgramInfoLog(id, length, &stored_length, message.data());
			}

			std::cerr << message << '\n';
		}
	}

	shader(const shader &) = delete;
	shader &operator=(const shader &) = delete;

	shader(shader &&other) : id{ other.id }
	{
		other.id = 0;
	}

	shader &operator=(shader &&other)
	{
		glDeleteProgram(id);
		id = other.id;
		other.id = 0;

		return *this;
	}

	~shader()
	{
		glDeleteProgram(id);
		id = 0;
	}

	GLuint id;
};

struct vao
{
	vao()
	{
		glGenVertexArrays(1, &id);
	}

	vao(const shader &) = delete;
	vao &operator=(const shader &) = delete;

	vao(vao &&other) : id{ other.id }
	{
		other.id = 0;
	}

	vao &operator=(vao &&other)
	{
		glDeleteVertexArrays(1, &id);
		id = other.id;
		other.id = 0;

		return *this;
	}

	~vao()
	{
		glDeleteVertexArrays(1, &id);
		id = 0;
	}

	GLuint id;
};

struct buffer
{
	buffer()
	{
		glGenBuffers(1, &id);
	}

	buffer(const buffer &) = delete;
	buffer &operator=(const buffer &) = delete;

	buffer(buffer &&other) : id{ other.id }
	{
		other.id = 0;
	}

	buffer &operator=(buffer &&other)
	{
		glDeleteBuffers(1, &id);
		id = other.id;
		other.id = 0;

		return *this;
	}

	~buffer()
	{
		glDeleteBuffers(1, &id);
		id = 0;
	}

	GLuint id;
};

struct texture
{
	texture() : width{}, height{}
	{
		glGenTextures(1, &id);
	}

	texture(GLenum target_format, const void *data, GLsizei _width, GLsizei _height, int channel_count, bool flip = false) : width{_width}, height{_height}
	{
		GLenum pixel_format;

		switch (channel_count)
		{
		case 1:
			pixel_format = GL_RED;
			break;
		case 2:
			pixel_format = GL_RG;
			break;
		case 3:
			pixel_format = GL_RGB;
			break;
		case 4:
			pixel_format = GL_RGBA;
			break;
		default:
			id = 0;
			return;
		}

		if (flip)
		{
			int w = width * channel_count;

			const unsigned char *old_data = reinterpret_cast<const unsigned char *>(data);
			unsigned char *new_data = new unsigned char[height * w];

			for (int y = 0; y < height; ++y)
			{
				auto *in = old_data + (height - y - 1) * w;
				std::copy(in, in + w, new_data + y * w);
			}

			data = new_data;
		}

		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, target_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		static float border_col[4]{0, 0, 0, 0};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_col);

		if (flip)
			delete[] reinterpret_cast<const unsigned char *>(data);
	}

	~texture()
	{
		glDeleteTextures(1, &id);
		width = height = 0;
		id = 0;
	}

	texture(const texture &) = delete;
	texture &operator=(const texture &) = delete;

	texture(texture &&other) : id{ other.id }, width{other.width}, height{other.height}
	{
		other.id = 0;
		other.width = other.height = 0;
	}

	texture &operator=(texture &&other)
	{
		glDeleteTextures(1, &id);
		id = other.id;
		other.id = 0;

		width = other.width;
		height = other.height;

		other.width = other.height = 0;

		return *this;
	}

	GLuint id;
	GLsizei width, height;
};

using vbo = buffer;
using ebo = buffer;
using ubo = buffer;
using ssbo = buffer;

#endif