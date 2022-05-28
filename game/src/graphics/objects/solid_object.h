#pragma once
#include "object.h"

namespace graphics
{
    class solid_object : public object
    {
    protected:
        vbo point_buffer;

        struct obj_program
        {
            inline static const char *vertex{
                "#version 430\n"
                "layout (location = 0) in vec2 position;\n"
                "uniform mat4 mv;\n"
                "uniform mat4 proj;\n"
                "uniform vec4 col;\n"
                "void main(){\n"
                "   gl_Position=proj*mv*vec4(position, 0.0, 1.0);\n"
                "}"
            };
            inline static const char *fragment{
                "#version 430\n"
                "uniform vec4 col;\n"
                "out vec4 out_col;\n"
                "void main(){\n"
                "   out_col=col;\n"
                "}"
            };

            shaders::program p;
            shaders::uniform mv;
            shaders::uniform proj;
            shaders::attribute position;
            shaders::uniform col;

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
                col = p.get_uniform("col");
                position = shaders::attribute(0);
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
        math::vec4 color;

    public:
        solid_object(application &app) : object(app), size{-1} {}
        ~solid_object() = default;

        template <typename T>
        void attach_shape(const T &p)
        {
            std::vector<math::vec2> pts;
            pts.reserve(p.size());
            for (const auto &pt : p)
                pts.push_back({pt.x, pt.y});
            size = static_cast<GLsizei>(p.size());

            get_vao().use();

            point_buffer = make_buffer<vbo_target>();
            point_buffer.attach_data(pts, GL_STATIC_DRAW);

            get_vao().quit();
        }

        void draw() const override
        {
            if (size == -1)
                return;

            const auto &p = get_program();

            get_vao().use();

            p.p.use();
            p.col.send_uniform(color);

            p.mv.send_uniform(mv_mat);
            p.proj.send_uniform(projection_mat);

            p.position.send_attribute(point_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

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
            p.col.send_uniform(color);

            p.mv.send_uniform(model * view);
            p.proj.send_uniform(proj);

            p.position.send_attribute(point_buffer, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

            glDrawArrays(GL_TRIANGLE_FAN, 0, size);

            get_vao().quit();
        }

        void set_color(const math::vec4 &col)
        {
            color = col;
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
