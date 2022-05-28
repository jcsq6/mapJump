#include "application.h"
#include "shaders.h"

#include "timer.h"

#include "solid_object.h"
#include "texture_object.h"

#include "bounding_box.h"

#include "writer.h"

#include "text.h"

#include "textures.h"
#include "state.h"

#include "saver.h"
#include "collision.h"

#include "file_dialog.h"

#include "simulate.h"

auto find(std::vector<state> &objs, const state &cur)
{
    for (auto it = objs.begin(); it != objs.end(); ++it)
    {
        if (physics::collides_non_inclusive({it->file_obj.min, it->size}, {cur.file_obj.min, cur.size}))
            return it;
    }
    return objs.end();
}

bool get_file(application &app, const math::mat<float, 4, 4> &ortho, math::dvec2 &mouse_pos, std::string &name);
void editor(application &app, std::string_view name, const math::mat<float, 4, 4> &ortho, math::dvec2 &mouse_pos);

int main(int argc, char *argv[])
{
    math::vec<int, 2> window_dims{1200, 675};

    auto ortho = math::ortho_mat(0, content_dims.x, 0, content_dims.y, -1, 1);

    application *app = application::get_instance("map jumper", window_dims.x, window_dims.y);
    input *input_handler = app->get_inputHandler();
    callback_handler *callback_manager = app->get_callbackManager();

    glfwSetWindowAspectRatio(app->get_window(), aspect_ratio.x, aspect_ratio.y);

    app->set_swapInterval(1);

    math::dvec2 mouse_pos;

    callback_manager->set_framebuffer([&window_dims](int width, int height)
                                      { window_dims = {width, height}; glViewport(0, 0, width, height); });

    callback_manager->set_cursor(
        [&](double x, double y)
        {
            mouse_pos.x = x * content_dims.x / window_dims.x;
            mouse_pos.y = (window_dims.y - y) * content_dims.y / window_dims.y;
        });

    do
    {
        std::string name;
        if (!get_file(*app, ortho, mouse_pos, name))
            return 0;
        editor(*app, name, ortho, mouse_pos);
    } while (true);
}

struct button
{
    static constexpr math::vec3 dormant_col{33.f / 255, 44.f / 255, 94.f / 255};
    static constexpr math::vec3 hover_col{63.f / 255, 81.f / 255, 181.f / 255};

    application &app;
    graphics::solid_object textre;
    physics::bounding_box bound;
    math::dvec2 text_loc;
    std::string_view txt;
    bool hovered;

    button(application &a, math::dvec2 center, math::dvec2 dims, std::string_view text, math::vec4 back_color) : app{a},
                                                                                                                 textre{app},
                                                                                                                 bound{center - dims / 2, dims},
                                                                                                                 text_loc{center.x - graphics::text_width(app, text, .75 * dims.y) / 2, bound.min.y + .25 * dims.y},
                                                                                                                 txt{text}, hovered{false}
    {
        textre.attach_shape(bound.points());
        textre.set_color(back_color);
    }

    void draw(const math::mat<float, 4, 4> &ortho)
    {
        textre.draw(math::identity(), math::identity(), ortho);
        graphics::print_text(app, txt, text_loc, .75 * bound.dims().y, hovered ? hover_col : dormant_col, ortho);
    }

    void get_hovered(math::dvec2 mouse_pos)
    {
        hovered = physics::is_inside(mouse_pos, bound);
    }
};

bool get_file(application &app, const math::mat<float, 4, 4> &ortho, math::dvec2 &mouse_pos, std::string &name)
{
    auto *input_handler = app.get_inputHandler();

    static button open(app, content_dims / 2 + math::dvec2{0, 200}, {300, 100}, "Open", {0.0, 0.0, 0.0, .2});
    static button create(app, content_dims / 2, {300, 100}, "Create", {0.0, 0.0, 0.0, .2});
    static button quit(app, content_dims / 2 - math::dvec2{0, 200}, {300, 100}, "Quit", {0.0, 0.0, 0.0, .2});

    const key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);
    const key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);

    while (!app.should_close())
    {
        input_handler->update_wait();

        if (escape.is_initialPress())
            glfwSetWindowShouldClose(app.get_window(), true);

        open.get_hovered(mouse_pos);
        create.get_hovered(mouse_pos);
        quit.get_hovered(mouse_pos);

        if (left_click.is_initialPress())
        {
            if ((open.hovered && open_file(name)) ||
                (create.hovered && save_file(name)))
            {
                return true;
            }
            if (quit.hovered)
                return false;
        }

        glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        open.draw(ortho);
        create.draw(ortho);
        quit.draw(ortho);

        app.swap_buffers();
    }

    return false;
}

void help(application &app, const math::mat<float, 4, 4> &ortho, math::dvec2 &mouse_pos)
{
    static button return_button(app, {content_dims.x / 2, content_dims.y - 75 * 13.3}, {800, 100}, "Return to pause menu", {0.0, 0.0, 0.0, .2});

    auto *input_handler = app.get_inputHandler();

    const key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);
    const key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);

    while (!app.should_close())
    {
        input_handler->update_wait();

        return_button.get_hovered(mouse_pos);

        if (escape.is_initialPress() ||
           (left_click.is_initialPress() && return_button.hovered))
        {
            return;
        }

        glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        graphics::print_text(app, "W/Up Arrow (increments color)",
            {(content_dims.x - graphics::text_width(app, "W/Up Arrow (increments color)", 50)) / 2, content_dims.y - 75 * 1},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "S/Down Arrow (decrements color)",
            {(content_dims.x - graphics::text_width(app, "S/Down Arrow (decrements color)", 50)) / 2, content_dims.y - 75 * 2},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "D/Right Arrow (rotates clockwise)",
            {(content_dims.x - graphics::text_width(app, "D/Right Arrow (rotates clockwise)", 50)) / 2, content_dims.y - 75 * 3},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "A/Left Arrow (rotates counterclockwise)",
            {(content_dims.x - graphics::text_width(app, "A/Left Arrow (rotates counterclockwise)", 50)) / 2, content_dims.y - 75 * 4},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Space (shows/hides grid)",
            {(content_dims.x - graphics::text_width(app, "Space (shows/hides grid)", 50)) / 2, content_dims.y - 75 * 5},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Left Click (places block)",
            {(content_dims.x - graphics::text_width(app, "Left Click (places block)", 50)) / 2, content_dims.y - 75 * 6},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Middle Click (copies block)",
            {(content_dims.x - graphics::text_width(app, "Middle Click (copies block)", 50)) / 2, content_dims.y - 75 * 7},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Right Click (removes block)",
            {(content_dims.x - graphics::text_width(app, "Right Click (removes block)", 50)) / 2, content_dims.y - 75 * 8},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Scroll (changes block type)",
            {(content_dims.x - graphics::text_width(app, "Scroll (changes block type)", 50)) / 2, content_dims.y - 75 * 9},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Drag the spawn/end anchors",
            {(content_dims.x - graphics::text_width(app, "Drag the spawn/end anchors", 50)) / 2, content_dims.y - 75 * 10},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "Auto saves every ten seconds",
            {(content_dims.x - graphics::text_width(app, "Auto saves every ten seconds", 50)) / 2, content_dims.y - 75 * 11},
            50, {0, 0, 0}, ortho);
        graphics::print_text(app, "You can place/remove blocks without releasing the mouse button",
            {(content_dims.x - graphics::text_width(app, "You can place/remove blocks without releasing the mouse button", 50)) / 2, content_dims.y - 75 * 12},
            50, {0, 0, 0}, ortho);

        return_button.draw(ortho);

        app.swap_buffers();
    }
}

// return true if exit's the editor
bool paused(application &app, const math::mat<float, 4, 4> &ortho, math::dvec2 &mouse_pos, const std::vector<state>& world, math::dvec2 spawn, math::dvec2 end, textures& texts)
{
    static button return_main_button(app, content_dims / 2 - math::dvec2{0, 300}, {800, 100}, "Return to main menu", {0.0, 0.0, 0.0, .2});
    static button help_button(app, content_dims / 2 - math::dvec2{0, 100}, {300, 100}, "Help", {0.0, 0.0, 0.0, .2});
    static button simulate_button(app, content_dims / 2 + math::dvec2{0, 100}, {450, 100}, "Simulate", {0.0, 0.0, 0.0, .2});
    static button return_button(app, content_dims / 2 + math::dvec2{0, 300}, {650, 100}, "Return to editor", {0.0, 0.0, 0.0, .2});

    auto *input_handler = app.get_inputHandler();

    const key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);
    const key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);

    while (!app.should_close())
    {
        input_handler->update_wait();

        if (escape.is_initialPress())
        {
            return true;
        }

        return_main_button.get_hovered(mouse_pos);
        help_button.get_hovered(mouse_pos);
        simulate_button.get_hovered(mouse_pos);
        return_button.get_hovered(mouse_pos);

        if (left_click.is_initialPress())
        {
            if (help_button.hovered)
            {
                help(app, ortho, mouse_pos);
                continue;
            }
            else if (return_main_button.hovered)
                return true;
            else if (simulate_button.hovered)
            {
                game::simulate(app, ortho, world, spawn, end, texts);
                continue;
            }
            else if (return_button.hovered)
                return false;
        }

        glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        return_main_button.draw(ortho);
        help_button.draw(ortho);
        simulate_button.draw(ortho);
        return_button.draw(ortho);

        app.swap_buffers();
    }

    return true;
}

void editor(application &app, std::string_view name, const math::mat<float, 4, 4> &ortho, math::dvec2 &mouse_pos)
{
    static textures txts(app);

    loader l;
    l.load(name);

    state cur{txts, game::block_color::blue, game::block_type::block};

    std::vector<state> objs = l.get_objs();
    for (auto &obj : objs)
        obj.update(txts, obj.file_obj.min);

    math::dvec2 spawn{l.get_spawn()};
    math::dvec2 end{l.get_end()};

    save saver(name);

    bool grid = false;

    auto *callback_manager = app.get_callbackManager();
    auto *input_handler = app.get_inputHandler();

    bool focused = true;

    callback_manager->set_scroll([&](double xscroll, double yscroll)
                                 { cur.file_obj.type = cur.file_obj.type + static_cast<int>(yscroll); });
    callback_manager->set_windowfocus([&focused](int is_focused){ focused = is_focused; });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);

    const key &a = input_handler->get_key(GLFW_KEY_A);
    const key &right = input_handler->get_key(GLFW_KEY_RIGHT);
    const key &d = input_handler->get_key(GLFW_KEY_D);
    const key &left = input_handler->get_key(GLFW_KEY_LEFT);

    const key &w = input_handler->get_key(GLFW_KEY_W);
    const key &up = input_handler->get_key(GLFW_KEY_UP);
    const key &down = input_handler->get_key(GLFW_KEY_DOWN);
    const key &s = input_handler->get_key(GLFW_KEY_S);

    const key &space = input_handler->get_key(GLFW_KEY_SPACE);

    const key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);
    const key &right_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_RIGHT);
    const key &middle_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_MIDDLE);

    bool spawn_grabbed = false;
    bool end_grabbed = false;

    // protect from returning from sub-menus
    bool left_initial = false;
    bool right_initial = false;

    countdown auto_saver(10);

    while (!app.should_close())
    {
        input_handler->update_wait();

        if (auto_saver.finished())
        {
            saver.mark(spawn, end, objs);
            auto_saver.start();
        }

        if (space.is_initialPress())
            grid = !grid;

        if (middle_click.is_initialPress())
        {
            auto it = find(objs, cur);
            if (it != objs.end())
                cur = *it;
        }

        if (a.is_initialPress() || left.is_initialPress())
            cur.file_obj.facing = game::turned_left(cur.file_obj.facing);
        else if (d.is_initialPress() || right.is_initialPress())
            cur.file_obj.facing = game::turned_right(cur.file_obj.facing);

        if (w.is_initialPress() || up.is_initialPress())
            ++cur.file_obj.col;
        if (s.is_initialPress() || down.is_initialPress())
            --cur.file_obj.col;

        if (left_click.is_initialPress())
            left_initial = true;
        if (right_click.is_initialPress())
            right_initial = true;

        if (left_click.is_pressed() && left_initial)
        {
            if (physics::collides_non_inclusive({spawn, cube_size}, {cur.file_obj.min, cube_size}))
            {
                spawn_grabbed = true;
            }
            else if (physics::collides_non_inclusive({end, cube_size}, {cur.file_obj.min, cube_size}))
            {
                end_grabbed = true;
            }
        }
        else
        {
            spawn_grabbed = end_grabbed = false;
        }

        if (!spawn_grabbed && !end_grabbed && ((left_click.is_pressed() && left_initial) || (right_click.is_pressed() && right_initial)))
        {
            do
            {
                auto it = find(objs, cur);
                if (it != objs.end())
                    objs.erase(it);
                else
                    break;
            } while (true);
        
            if (left_click.is_pressed())
                objs.push_back(cur);
        }

        if (escape.is_initialPress() || !focused)
        {
            if (paused(app, ortho, mouse_pos, objs, spawn, end, txts))
                break;
            left_initial = right_initial = false;
            continue;
        }

        cur.update(txts, mouse_pos);

        glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        txts.background.draw(math::identity(), math::identity(), ortho);

        if (grid)
            draw_grid(app, ortho);

        for (auto &obj : objs)
            obj.draw(ortho);

        if (spawn_grabbed)
        {
            spawn = mouse_pos;
            force_grid(spawn);
        }
        else if (end_grabbed)
        {
            end = mouse_pos;
            force_grid(end);
        }
        else
        {
            cur.draw(ortho);
        }

        txts.spawn_anchor.draw(math::translate<float>(spawn.x, spawn.y, 0), math::identity(), ortho);
        txts.end_anchor.draw(math::translate<float>(end.x, end.y, 0), math::identity(), ortho);

        app.swap_buffers();
    }
    saver.mark(spawn, end, objs);
    callback_manager->set_scroll([&](double xscroll, double yscroll) {});
}