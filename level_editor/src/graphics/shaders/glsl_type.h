#pragma once
#include "mat.h"
#include "macros.h"
#include <GL/glew.h>

GRAPHICS_BEG
SHADERS_BEG

enum class glsl_type : GLenum
{
    bool_t = GL_BOOL,
    int_t = GL_INT,
    uint_t = GL_UNSIGNED_INT,
    float_t = GL_FLOAT,
    double_t = GL_DOUBLE,
    bvec2_t,
    bvec3_t,
    bvec4_t,
    ivec2_t,
    ivec3_t,
    ivec4_t,
    uvec2_t,
    uvec3_t,
    uvec4_t,
    vec2_t,
    vec3_t,
    vec4_t,
    dvec2_t,
    dvec3_t,
    dvec4_t,
    mat2_t,
    mat3_t,
    mat4_t,
    mat2x3_t,
    mat2x4_t,
    mat3x2_t,
    mat3x4_t,
    mat4x2_t,
    mat4x3_t,
};

template <typename T>
constexpr glsl_type type;

template <>
constexpr glsl_type type<bool> = glsl_type::bool_t;

template <>
constexpr glsl_type type<math::vec<int, 1>> = glsl_type::int_t;

template <>
constexpr glsl_type type<int> = glsl_type::int_t;

template <>
constexpr glsl_type type<math::vec<unsigned int, 1>> = glsl_type::uint_t;

template <>
constexpr glsl_type type<unsigned int> = glsl_type::uint_t;

template <>
constexpr glsl_type type<math::vec<float, 1>> = glsl_type::float_t;

template <>
constexpr glsl_type type<float> = glsl_type::float_t;

template <>
constexpr glsl_type type<math::vec<double, 1>> = glsl_type::double_t;

template <>
constexpr glsl_type type<double> = glsl_type::double_t;

template <>
constexpr glsl_type type<math::vec<bool, 2>> = glsl_type::bvec2_t;

template <>
constexpr glsl_type type<math::vec<bool, 3>> = glsl_type::bvec3_t;

template <>
constexpr glsl_type type<math::vec<bool, 4>> = glsl_type::bvec4_t;

template <>
constexpr glsl_type type<math::vec<int, 2>> = glsl_type::ivec2_t;

template <>
constexpr glsl_type type<math::vec<int, 3>> = glsl_type::ivec3_t;

template <>
constexpr glsl_type type<math::vec<int, 4>> = glsl_type::ivec4_t;

template <>
constexpr glsl_type type<math::vec<unsigned int, 2>> = glsl_type::uvec2_t;

template <>
constexpr glsl_type type<math::vec<unsigned int, 3>> = glsl_type::uvec3_t;

template <>
constexpr glsl_type type<math::vec<unsigned int, 4>> = glsl_type::uvec4_t;

template <>
constexpr glsl_type type<math::vec<float, 2>> = glsl_type::vec2_t;

template <>
constexpr glsl_type type<math::vec<float, 3>> = glsl_type::vec3_t;

template <>
constexpr glsl_type type<math::vec<float, 4>> = glsl_type::vec4_t;

template <>
constexpr glsl_type type<math::dvec2> = glsl_type::dvec2_t;

template <>
constexpr glsl_type type<math::vec<double, 3>> = glsl_type::dvec3_t;

template <>
constexpr glsl_type type<math::vec<double, 4>> = glsl_type::dvec4_t;

template <>
constexpr glsl_type type<math::mat<float, 2, 2>> = glsl_type::mat2_t;

template <>
constexpr glsl_type type<math::mat<float, 3, 3>> = glsl_type::mat3_t;

template <>
constexpr glsl_type type<math::mat<float, 4, 4>> = glsl_type::mat4_t;

template <>
constexpr glsl_type type<math::mat<float, 2, 3>> = glsl_type::mat2x3_t;

template <>
constexpr glsl_type type<math::mat<float, 2, 4>> = glsl_type::mat2x4_t;

template <>
constexpr glsl_type type<math::mat<float, 3, 2>> = glsl_type::mat3x2_t;

template <>
constexpr glsl_type type<math::mat<float, 3, 4>> = glsl_type::mat3x4_t;

template <>
constexpr glsl_type type<math::mat<float, 4, 2>> = glsl_type::mat4x2_t;

template <>
constexpr glsl_type type<math::mat<float, 4, 3>> = glsl_type::mat4x3_t;

template <glsl_type gt, typename... Ts>
void glUniform(Ts... args)
{
    if constexpr (gt == glsl_type::bool_t)
        glUniform1i(args...);
    else if constexpr (gt == glsl_type::int_t)
        glUniform1i(args...);
    else if constexpr (gt == glsl_type::uint_t)
        glUniform1ui(args...);
    else if constexpr (gt == glsl_type::float_t)
        glUniform1f(args...);
    else if constexpr (gt == glsl_type::double_t)
        glUniform1d(args...);
    else if constexpr (gt == glsl_type::bvec2_t)
        glUniform2i(args...);
    else if constexpr (gt == glsl_type::bvec3_t)
        glUniform3i(args...);
    else if constexpr (gt == glsl_type::bvec4_t)
        glUniform4i(args...);
    else if constexpr (gt == glsl_type::ivec2_t)
        glUniform2i(args...);
    else if constexpr (gt == glsl_type::bvec3_t)
        glUniform3i(args...);
    else if constexpr (gt == glsl_type::bvec4_t)
        glUniform4i(args...);
    else if constexpr (gt == glsl_type::uvec2_t)
        glUniform2ui(args...);
    else if constexpr (gt == glsl_type::uvec3_t)
        glUniform3ui(args...);
    else if constexpr (gt == glsl_type::uvec4_t)
        glUniform4ui(args...);
    else if constexpr (gt == glsl_type::vec2_t)
        glUniform2f(args...);
    else if constexpr (gt == glsl_type::vec3_t)
        glUniform3f(args...);
    else if constexpr (gt == glsl_type::vec4_t)
        glUniform4f(args...);
    else if constexpr (gt == glsl_type::dvec2_t)
        glUniform2d(args...);
    else if constexpr (gt == glsl_type::dvec3_t)
        glUniform3d(args...);
    else if constexpr (gt == glsl_type::dvec4_t)
        glUniform4d(args...);
}

template <glsl_type gt, typename... Ts>
void glUniformv(GLint location, GLsizei count, Ts... args)
{
    if constexpr (gt == glsl_type::bvec2_t)
        glUniform2iv(location, count, args...);
    else if constexpr (gt == glsl_type::bvec3_t)
        glUniform3iv(location, count, args...);
    else if constexpr (gt == glsl_type::bvec4_t)
        glUniform4iv(location, count, args...);
    else if constexpr (gt == glsl_type::ivec2_t)
        glUniform2iv(location, count, args...);
    else if constexpr (gt == glsl_type::bvec3_t)
        glUniform3iv(location, count, args...);
    else if constexpr (gt == glsl_type::bvec4_t)
        glUniform4iv(location, count, args...);
    else if constexpr (gt == glsl_type::uvec2_t)
        glUniform2uiv(location, count, args...);
    else if constexpr (gt == glsl_type::uvec3_t)
        glUniform3uiv(location, count, args...);
    else if constexpr (gt == glsl_type::uvec4_t)
        glUniform4uiv(location, count, args...);
    else if constexpr (gt == glsl_type::vec2_t)
        glUniform2fv(location, count, args...);
    else if constexpr (gt == glsl_type::vec3_t)
        glUniform3fv(location, count, args...);
    else if constexpr (gt == glsl_type::vec4_t)
        glUniform4fv(location, count, args...);
    else if constexpr (gt == glsl_type::dvec2_t)
        glUniform2dv(location, count, args...);
    else if constexpr (gt == glsl_type::dvec3_t)
        glUniform3dv(location, count, args...);
    else if constexpr (gt == glsl_type::dvec4_t)
        glUniform4dv(location, count, args...);
    else if constexpr (gt == glsl_type::mat2_t)
        glUniformMatrix2fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat3_t)
        glUniformMatrix3fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat4_t)
        glUniformMatrix4fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat2x3_t)
        glUniformMatrix2x3fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat2x4_t)
        glUniformMatrix2x4fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat3x2_t)
        glUniformMatrix3x2fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat3x4_t)
        glUniformMatrix3x4fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat4x2_t)
        glUniformMatrix4x2fv(location, count, args...);
    else if constexpr (gt == glsl_type::mat4x3_t)
        glUniformMatrix4x3fv(location, count, args...);
}
SHADERS_END
GRAPHICS_END