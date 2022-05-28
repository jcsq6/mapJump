#pragma once
#include "mat.h"
#include "application.h"

#include "texture.h"
#include "buffers.h"
#include "shaders.h"
//#include "arial.h"

#include <string>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H

struct obj_program
{
    inline static const char *vertex{
        "#version 430\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 text_coord;\n"
        "uniform mat4 proj;\n"
        "out vec2 vert_text_coord;\n"
        "void main(){\n"
        "   gl_Position=proj*vec4(position, 0.0, 1.0);\n"
        "   vert_text_coord = text_coord;\n"
        "}"};
    inline static const char *fragment{
        "#version 430\n"
        "layout (binding=0) uniform sampler2D samp;\n"
        "uniform vec3 text_color;\n"
        "in vec2 vert_text_coord;\n"
        "out vec4 out_col;\n"
        "void main(){\n"
        "   out_col=vec4(text_color.xyz, texture(samp, vert_text_coord).r);\n"
        "}"};

    graphics::shaders::program p;
    graphics::shaders::uniform proj;
    graphics::shaders::uniform text_color;
    graphics::shaders::attribute position;
    graphics::shaders::attribute text_coord;

    obj_program()
    {
        graphics::shaders::shader vert(GL_VERTEX_SHADER);
        vert.attach_source(vertex);
        vert.compile();

        graphics::shaders::shader frag(GL_FRAGMENT_SHADER);
        frag.attach_source(fragment);
        frag.compile();

        p = graphics::shaders::program(vert, frag);
        text_color = p.get_uniform("text_color");
        proj = p.get_uniform("proj");
        position = graphics::shaders::attribute(0);
        text_coord = graphics::shaders::attribute(1);
    }
};

struct character
{
    graphics::texture texture;
    math::vec<int, 2> size;
    math::vec<int, 2> offset;
    unsigned int advance;
};

struct library
{
    static constexpr int height = 128;

    library(application &app) :
        program(),
        array_object(),
        point_buffer(),
        text_buffer(),
        chars{}
    {
        math::vec2 text_coords[4]
        {
            {0, 1},
            {1, 1},
            {1, 0},
            {0, 0},
            // //
            // {0, 0},
            // {1, 0},
            // {1, 1},
            // {0, 1},
        };

        array_object.use();
        
        point_buffer = graphics::make_buffer<vbo_target>();
        text_buffer = graphics::make_buffer<vbo_target>();
        text_buffer.attach_data(text_coords, GL_DYNAMIC_DRAW);

        program.position.send_attribute(point_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        program.text_coord.send_attribute(text_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        FT_Library lib;
        if (FT_Init_FreeType(&lib))
            throw std::runtime_error("could not initialize freetype");

        FT_Face face;
        if (FT_New_Face(lib, "assets/arial.ttf", 0, &face))
            throw std::runtime_error("could not load font");
        
        //FT_New_Memory_Face(lib, reinterpret_cast<const FT_Byte *>(arial), std::size(arial), 0, &face)

        FT_Set_Pixel_Sizes(face, 0, height);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (int i = 0; i < 256; ++i)
        {
            if (FT_Load_Char(face, i, FT_LOAD_RENDER))
                continue;

            chars[i].texture.load(app, GL_RED, face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows);
            chars[i].size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
            chars[i].offset = {face->glyph->bitmap_left, face->glyph->bitmap_top};
            chars[i].advance = face->glyph->advance.x;
        }

        FT_Done_Face(face);
        FT_Done_FreeType(lib);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    }

    obj_program program;

    graphics::vao array_object;
    graphics::vbo point_buffer;
    graphics::vbo text_buffer;

    character chars[256];
};

namespace graphics
{
    void print_text(application& app, std::string_view text, math::vec2 loc, float height, math::vec3 color, const math::mat<float, 4, 4>& ortho)
    {
        static library lib(app);

        float scale = height / lib.height;

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glActiveTexture(GL_TEXTURE0);

        lib.array_object.use();
        lib.text_buffer.use();
        lib.program.p.use();
        lib.program.proj.send_uniform(ortho);
        lib.program.text_color.send_uniform(color);

        for (char c : text)
        {
            character& o = lib.chars[c];

            float x = loc.x + o.offset.x;
            float y = loc.y - (o.size.y - o.offset.y) * scale;

            float w = static_cast<float>(o.size.x) * scale;
            float h = static_cast<float>(o.size.y) * scale;

            math::vec2 verts[4]
            {
                {x, y},
                {x + w, y},
                {x + w, y + h},
                {x, y + h},
            };

            o.texture.use();

            lib.point_buffer.attach_data(verts, GL_DYNAMIC_DRAW);
            lib.point_buffer.use();

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            o.texture.quit();

            loc.x += (o.advance >> 6) * scale;
        }

        lib.point_buffer.quit();
        lib.array_object.quit();
    }
}