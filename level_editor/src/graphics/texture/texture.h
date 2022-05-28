#pragma once
#include <GL/glew.h>
#include <string>

#include "application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>

GRAPHICS_BEG
class texture
{
    GLuint id;
    int width;
    int height;
    int nr_channels;

    void destroy()
    {
        glDeleteTextures(1, &id);
    }

public:
    texture() : id{} {}
    texture(std::string_view file_name) : id{}
    {
        load(file_name);
    }

    texture(texture &&other) : id{other.id}
    {
        other.id = 0;
    }

    texture &operator=(texture &&other)
    {
        destroy();
        id = other.id;
        other.id = 0;
        return *this;
    }

    void set_parameter(GLenum property, GLint value)
    {
        use();
        glTexParameteri(GL_TEXTURE_2D, property, value);
        quit();
    }

    void set_parameter(GLenum property, const GLint *value)
    {
        use();
        glTexParameteriv(GL_TEXTURE_2D, property, value);
        quit();
    }

    void set_parameter(GLenum property, GLfloat value)
    {
        use();
        glTexParameterf(GL_TEXTURE_2D, property, value);
        quit();
    }

    void set_parameter(GLenum property, const GLfloat *value)
    {
        use();
        glTexParameterfv(GL_TEXTURE_2D, property, value);
        quit();
    }

    int get_width() const
    {
        return width;
    }

    int get_height() const
    {
        return height;
    }

    int get_channels() const
    {
        return nr_channels;
    }

    void load(std::string_view file_name)
    {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(file_name.data(), &width, &height, &nr_channels, 0);

        GLint format;

        switch (nr_channels)
        {
        case 1:
            format = GL_RED;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            throw std::runtime_error("Unrecognized image format for image " + std::string(file_name));
        }

        glGenTextures(1, &id);
        use();
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        quit();

        stbi_image_free(data);
    }

    void load(GLenum pixel_format, const void *data, GLsizei width, GLsizei height)
    {
        this->width = width;
        this->height = height;

        switch (pixel_format)
        {
        case GL_RED:
            nr_channels = 1;
            break;
        case GL_RG:
            nr_channels = 2;
            break;
        case GL_RGB:
            nr_channels = 3;
            break;
        case GL_RGBA:
            nr_channels = 4;
            break;
        default:
            throw std::runtime_error("Unrecognized image format");
        }

        glGenTextures(1, &id);
        use();
        glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        quit();
    }

    ~texture()
    {
        destroy();
    }

    GLuint get_id() const
    {
        return id;
    }

    static void quit()
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // make sure to activate texture unit before this
    void use() const
    {
        glBindTexture(GL_TEXTURE_2D, id);
    }
};
GRAPHICS_END