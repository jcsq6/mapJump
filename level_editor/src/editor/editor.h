#pragma once
#include "saver.h"
#include "application.h"
#include "state.h"
#include "simulate.h"
#include "timer.h"
#include "file_dialog.h"
#include "menu.h"

auto find(std::vector<editor::state> &objs, const editor::state &cur)
{
    for (auto it = objs.begin(); it != objs.end(); ++it)
    {
        if (physics::collides_non_inclusive({it->file_obj.min, it->size}, {cur.file_obj.min, cur.size}))
            return it;
    }
    return objs.end();
}

EDITOR_BEG
void help_menu()
{
    static graphics::application& app = *graphics::application::get_instance();

    static graphics::button return_button({game::content_dims.x / 2, game::content_dims.y - 75 * 12.5}, {800, 100}, "Return to pause menu", {0.0, 0.0, 0.0, .2});

    auto *input_handler = app.get_inputHandler();

    const graphics::key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);
    const graphics::key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);

    while (!app.should_close())
    {
        input_handler->update_wait();

        auto mouse_pos = input_handler->mouse_pos();

        mouse_pos.x = mouse_pos.x * game::content_dims.x / game::window_dims.x;
        mouse_pos.y = (game::window_dims.y - mouse_pos.y) * game::content_dims.y / game::window_dims.y;

        return_button.get_hovered(mouse_pos);

        if (escape.is_initialPress() ||
           (left_click.is_initialPress() && return_button.hovered))
        {
            return;
        }

        glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        graphics::print_text("W/Up Arrow (increments color)",
            {(game::content_dims.x - graphics::text_width("W/Up Arrow (increments color)", 50)) / 2, game::content_dims.y - 75 * 1},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("S/Down Arrow (decrements color)",
            {(game::content_dims.x - graphics::text_width("S/Down Arrow (decrements color)", 50)) / 2, game::content_dims.y - 75 * 2},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("D/Right Arrow (rotates clockwise)",
            {(game::content_dims.x - graphics::text_width("D/Right Arrow (rotates clockwise)", 50)) / 2, game::content_dims.y - 75 * 3},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("A/Left Arrow (rotates counterclockwise)",
            {(game::content_dims.x - graphics::text_width("A/Left Arrow (rotates counterclockwise)", 50)) / 2, game::content_dims.y - 75 * 4},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("Left Click (places block)",
            {(game::content_dims.x - graphics::text_width("Left Click (places block)", 50)) / 2, game::content_dims.y - 75 * 5},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("Middle Click (copies block)",
            {(game::content_dims.x - graphics::text_width("Middle Click (copies block)", 50)) / 2, game::content_dims.y - 75 * 6},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("Right Click (removes block)",
            {(game::content_dims.x - graphics::text_width("Right Click (removes block)", 50)) / 2, game::content_dims.y - 75 * 7},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("Scroll (changes block type)",
            {(game::content_dims.x - graphics::text_width("Scroll (changes block type)", 50)) / 2, game::content_dims.y - 75 * 8},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("Drag the spawn/end anchors",
            {(game::content_dims.x - graphics::text_width("Drag the spawn/end anchors", 50)) / 2, game::content_dims.y - 75 * 9},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("Auto saves every ten seconds",
            {(game::content_dims.x - graphics::text_width("Auto saves every ten seconds", 50)) / 2, game::content_dims.y - 75 * 10},
            50, {0, 0, 0}, game::ortho);
        graphics::print_text("You can place/remove blocks without releasing the mouse button",
            {(game::content_dims.x - graphics::text_width("You can place/remove blocks without releasing the mouse button", 50)) / 2, game::content_dims.y - 75 * 11},
            50, {0, 0, 0}, game::ortho);

        return_button.draw();

        app.swap_buffers();
    }
}

void edit_app(std::string_view name)
{
    static graphics::application& app = *graphics::application::get_instance();
    static graphics::game_textures& txts = graphics::game_textures::get_instance();

    graphics::menu pause(100);
    auto* return_main = pause.add_button("Return to main menu");
    auto* simulate = pause.add_button("Simulate");
    auto* help = pause.add_button("Help");
    auto* return_editor = pause.add_button("Return to editor");

    loader l;
    l.load(name);

    state cur{game::block_color::blue, game::block_type::block};

    std::vector<state> objs = l.get_objs();
    for (auto &obj : objs)
        obj.update(obj.file_obj.min);

    math::dvec2 spawn{l.get_spawn()};
    math::dvec2 end{l.get_end()};

    save saver(name);

    auto *callback_manager = app.get_callbackManager();
    auto *input_handler = app.get_inputHandler();

    bool focused = true;

    callback_manager->set_scroll([&](double xscroll, double yscroll)
                                 { cur.file_obj.type = cur.file_obj.type + static_cast<int>(yscroll); });
    callback_manager->set_windowfocus([&focused](int is_focused){ focused = is_focused; });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const graphics::key &escape = input_handler->get_key(GLFW_KEY_ESCAPE);

    const graphics::key &a = input_handler->get_key(GLFW_KEY_A);
    const graphics::key &right = input_handler->get_key(GLFW_KEY_RIGHT);
    const graphics::key &d = input_handler->get_key(GLFW_KEY_D);
    const graphics::key &left = input_handler->get_key(GLFW_KEY_LEFT);

    const graphics::key &w = input_handler->get_key(GLFW_KEY_W);
    const graphics::key &up = input_handler->get_key(GLFW_KEY_UP);
    const graphics::key &down = input_handler->get_key(GLFW_KEY_DOWN);
    const graphics::key &s = input_handler->get_key(GLFW_KEY_S);

    const graphics::key &left_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_LEFT);
    const graphics::key &right_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_RIGHT);
    const graphics::key &middle_click = input_handler->get_mouse_button(GLFW_MOUSE_BUTTON_MIDDLE);

    bool spawn_grabbed = false;
    bool end_grabbed = false;

    // protect from returning from sub-menus
    bool left_initial = false;
    bool right_initial = false;

    utilities::countdown auto_saver(10);

    while (!app.should_close())
    {
        input_handler->update_wait();

        if (auto_saver.finished())
        {
            saver.mark(spawn, end, objs);
            auto_saver.start();
        }

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
            if (physics::collides_non_inclusive({spawn, game::cube_size}, {cur.file_obj.min, game::cube_size}))
            {
                spawn_grabbed = true;
            }
            else if (physics::collides_non_inclusive({end, game::cube_size}, {cur.file_obj.min, game::cube_size}))
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
            do
            {
                auto *picked = pause.run();
                if (picked == return_main || picked == &graphics::menu::escape_clicked)
                    return;
                else if (picked == simulate)
                    game::simulate(objs, spawn, end);
                else if (picked == help)
                    help_menu();
                else
                    break;

            } while (true);

            left_initial = right_initial = false;
            continue;
        }

        auto mouse_pos = input_handler->mouse_pos();

        mouse_pos.x = mouse_pos.x * game::content_dims.x / game::window_dims.x;
        mouse_pos.y = (game::window_dims.y - mouse_pos.y) * game::content_dims.y / game::window_dims.y;

        cur.update(mouse_pos);

        glClearColor(148.f / 255, 200.f / 255, 238.f / 255, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        txts.background.draw(math::identity(), math::identity(), game::ortho);

        for (auto &obj : objs)
            obj.draw(game::ortho);

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
            cur.draw(game::ortho);
        }

        txts.spawn_anchor.draw(math::translate<float>(spawn.x, spawn.y, 0), math::identity(), game::ortho);
        txts.end_anchor.draw(math::translate<float>(end.x, end.y, 0), math::identity(), game::ortho);

        app.swap_buffers();
    }
    saver.mark(spawn, end, objs);
    callback_manager->set_scroll([&](double xscroll, double yscroll) {});
}

void run()
{
    graphics::menu load_menu(100);
    auto* open = load_menu.add_button("Open");
    auto* create = load_menu.add_button("Create");
    auto* quit = load_menu.add_button("Quit");
    do
    {
        std::string name;
        auto* res = load_menu.run();
        if ((res == open && !utilities::open_file(name)) ||
            (res == create && !utilities::save_file(name))
           )
        {
            continue;
        }

        if (res == quit || res == &graphics::menu::escape_clicked || res == &graphics::menu::window_closed)
            break;

        edit_app(name);
    } while (true);
}

EDITOR_END