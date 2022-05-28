#pragma once
#include "object.h"
#include "texture.h"

namespace graphics
{
    class texture_object : public object
    {
    protected:
        vbo point_buffer;
        vbo text_buffer;

        struct obj_program
        {
            inline static const char *vertex{
                "#version 430\n"
                "layout (location = 0) in vec2 position;\n"
                "layout (location = 1) in vec2 text_coord;\n"
                "uniform mat4 mv;\n"
                "uniform mat4 proj;\n"
                "out vec2 vert_text_coord;\n"
                "void main(){\n"
                "   gl_Position=proj*mv*vec4(position, 0.0, 1.0);\n"
                "   vert_text_coord = text_coord;\n"
                "}"
            };
            inline static const char *fragment{
                "#version 430\n"
                "layout (binding=0) uniform sampler2D samp;\n"
                "in vec2 vert_text_coord;\n"
                "out vec4 out_col;\n"
                "void main(){\n"
                "   out_col=texture(samp, vert_text_coord);\n"
                "}"
            };

            shaders::program p;
            shaders::uniform mv;
            shaders::uniform proj;
            shaders::attribute position;
            shaders::attribute text_coord;

            obj_program()
            {
                shaders::shader vert(GL_VERTEX_SHADER);
                vert.attach_source(vertex);
                vert.compile();

                shaders::shader frag(GL_FRAGMENT_SHADER);
                frag.attach_source(fragment);
                frag.compile();

                p = shaders::program(vert, frag);
                mv = p.get_uniform("mv");
                proj = p.get_uniform("proj");
                position = shaders::attribute(0);
                text_coord = shaders::attribute(1);
            }
        };

        inline static vao& get_vao()
        {
            static vao v;
            return v;
        }

        inline static const obj_program &get_program()
        {
            static obj_program p;
            return p;
        }

        GLsizei size;

        math::mat<float, 4, 4> mv_mat;
        math::mat<float, 4, 4> projection_mat;

        texture t;
    public:
        texture_object(application &app, std::string_view file_name) :
            object(app), size{-1},
            mv_mat{math::identity()}, projection_mat{math::identity()},
            t(app, file_name)
        {}
        ~texture_object() = default;

        template <typename T>
        void attach_shape(const T &p)
        {
            std::vector<math::vec2> pts;
            pts.reserve(p.size());
            for (const auto &pt : p)
                pts.push_back({pt.x, pt.y});

            auto new_size = static_cast<GLsizei>(p.size());
            if (size != -1 && new_size != size)
                throw std::runtime_error("Texture shape size and point shape size are not the same");
            size = new_size;

            get_vao().use();

            point_buffer = make_buffer<vbo_target>();
            point_buffer.attach_data(pts, GL_STATIC_DRAW);

            get_vao().quit();
        }

        template <typename T>
        void attach_texture_shape(const T &p)
        {
            std::vector<math::vec2> pts;
            pts.reserve(p.size());
            for (const auto &pt : p)
                pts.push_back({pt.x, pt.y});

            auto new_size = static_cast<GLsizei>(p.size());
            if (size != -1 && new_size != size)
                throw std::runtime_error("Texture shape size and point shape size are not the same");
            size = new_size;

            get_vao().use();

            text_buffer = make_buffer<vbo_target>();
            text_buffer.attach_data(pts, GL_STATIC_DRAW);

            get_vao().quit();
        }

        template<typename T>
        void set_texture_parameter(GLenum property, const T* value)
        {
            t.set_parameter(property, value);
        }

        template<typename T>
        void set_texture_parameter(GLenum property, T value)
        {
            t.set_parameter(property, value);
        }

        int texture_width() const
        {
            return t.get_width();
        }

        int texture_height() const
        {
            return t.get_height();
        }

        int texture_channels() const
        {
            return t.get_channels();
        }

        void draw() const override
        {
            if (size == -1)
                return;

            const auto &p = get_program();

            get_vao().use();

            p.p.use();

            p.mv.send_uniform(mv_mat);
            p.proj.send_uniform(projection_mat);

            p.position.send_attribute(point_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            p.text_coord.send_attribute(text_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

            glActiveTexture(GL_TEXTURE0);
            t.use();

            glDrawArrays(GL_TRIANGLE_FAN, 0, size);

            get_vao().quit();
        }

        void draw(const math::mat<float, 4, 4> &model, const math::mat<float, 4, 4> &view, const math::mat<float, 4, 4> &proj)
        {
            if (size == -1)
                return;

            const auto &p = get_program();

            get_vao().use();

            p.p.use();

            p.mv.send_uniform(model * view);
            p.proj.send_uniform(proj);

            p.position.send_attribute(point_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            p.text_coord.send_attribute(text_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

            glActiveTexture(GL_TEXTURE0);
            t.use();

            glDrawArrays(GL_TRIANGLE_FAN, 0, size);

            get_vao().quit();
        }

        void set_mvMat(const math::mat<float, 4, 4> &model, const math::mat<float, 4, 4> &view)
        {
            mv_mat = model * view;
        }

        void set_projMat(const math::mat<float, 4, 4> &proj)
        {
            projection_mat = proj;
        }
    };
}
