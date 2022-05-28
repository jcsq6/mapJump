#pragma once
#include <vector>
#include <string>
#include <iostream>

#include "glsl_type.h"
#include "buffers.h"

namespace graphics
{
    namespace shaders
    {
        template <typename T>
        struct is_mat : std::false_type
        {
        };

        template <typename T, size_t w, size_t h>
        struct is_mat<math::mat<T, w, h>> : std::true_type
        {
        };

        template <typename T>
        struct is_vec : std::false_type
        {
        };

        template <typename T, size_t length>
        struct is_vec<math::vec<T, length>> : std::true_type
        {
        };

        class uniform
        {
            int loc;

        public:
            uniform(int location = 0) : loc{location} {}

            // specilization for scalars
            template <typename T>
            std::enable_if_t<!is_vec<T>::value && !is_mat<T>::value> send_uniform(T val) const
            {
                glUniform(loc, val);
            }

            // overload for vecs and matrices
            template <typename T>
            std::enable_if_t<is_vec<T>::value || is_mat<T>::value> send_uniform(const T &val) const
            {
                if constexpr (is_mat<T>::value)
                    glUniformv<type<T>>(loc, 1, GL_FALSE, math::value(val));
                else
                    glUniformv<type<T>>(loc, 1, math::value(val));
            }
        };

        class attribute
        {
            unsigned int loc;

        public:
            attribute(unsigned int location = 0) : loc{location} {}

            void send_attribute(const vbo &b, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer) const
            {
                b.use();
                glVertexAttribPointer(loc, size, type, normalized, stride, pointer);
                glEnableVertexAttribArray(loc);
            }
        };

        class shader
        {
            GLuint id;

        public:
            shader() : id{} {}
            shader(shader &&o) : id{o.id}
            {
                o.id = 0;
            }
            shader(const shader &) = delete;
            shader(int shader_type) : id{glCreateShader(shader_type)}
            {
            }

            shader &operator=(shader &&o)
            {
                if (id)
                    glDeleteShader(id);
                id = o.id;
                o.id = 0;
                return *this;
            }

            shader &operator=(const shader &) = delete;

            void attach_source(const char *src) const
            {
                glShaderSource(id, 1, &src, nullptr);
            }

            void compile() const
            {
                glCompileShader(id);

                GLint completed{};

                glGetShaderiv(id, GL_COMPILE_STATUS, &completed);
                if (completed == GL_FALSE)
                {
                    std::cout << "shader compilation failed!\n";
                    int length{};
                    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
                    if (length)
                    {
                        int d{};
                        char *log = new char[length];
                        glGetShaderInfoLog(id, length, &d, log);
                        std::cout << "shader Log: " << log << "\n";
                        delete[] log;
                    }
                }
            }

            void attach_toProgram(GLuint prog)
            {
                glAttachShader(prog, id);
                glDeleteShader(id);
                id = 0;
            }

            ~shader()
            {
                glDeleteShader(id);
                id = 0;
            }
        };

        class program
        {
            GLuint id;

        public:
            program() : id{} {}

            program(program &&o) : id{o.id}
            {
                o.id = 0;
            }

            program(const program &) = delete;

            template <typename... Shs>
            program(Shs &&...shaders) : id{glCreateProgram()}
            {
                (shaders.attach_toProgram(id), ...);

                glLinkProgram(id);

                GLint completed = 0;

                glGetProgramiv(id, GL_LINK_STATUS, &completed);
                if (completed == GL_FALSE)
                {
                    int length{};

                    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
                    if (length)
                    {
                        int d{};
                        char *log{new char[length]};
                        glGetProgramInfoLog(id, length, &d, log);
                        std::cout << "Program Log: " << log << "\n";
                        delete[] log;
                    }
                }
            }

            program &operator=(program &&o)
            {
                if (id)
                    glDeleteProgram(id);
                id = o.id;
                o.id = 0;
                return *this;
            }

            program &operator=(const program &) = delete;

            uniform get_uniform(const char *name) const
            {
                return glGetUniformLocation(id, name);
            }

            attribute get_attribute(const char *name) const
            {
                return glGetAttribLocation(id, name);
            }

            void use() const
            {
                glUseProgram(id);
            }

            static void define_attribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer)
            {
                glVertexAttribPointer(index, size, type, normalized, stride, pointer);
            }

            ~program()
            {
                glDeleteProgram(id);
                id = 0;
            }
        };
    }
}