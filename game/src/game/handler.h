#pragma once
#include "debug.h"
#include "game_obj.h"
#include "loader.h"
#include <vector>

namespace game
{
    class game_obj;
    
    class handler
    {
        enum class state : bool
        {
            blue,
            red,
        };


        struct block
        {
            physics::bounding_box bound;
            std::vector<game_obj*> sub_objs;
        };

        application &app;

        state game_state;

        math::dvec2 initial_position;
        math::vec<int, 2> window_size;

        player user;
        countdown jump_buffered_countdown;

        std::vector<game_obj> blue_objs;
        std::vector<game_obj> red_objs;
        std::vector<game_obj> neutral_objs;

        std::vector<block> blue_blocks;
        std::vector<block> red_blocks;
        std::vector<block> neutral_blocks;
        block window_blocks[4];

        graphics::texture_object blue_cube;
        graphics::texture_object blue_cube_fade;
        graphics::texture_object blue_jump;
        graphics::texture_object blue_spike;
        graphics::texture_object blue_spike_fade;
    
        graphics::texture_object red_cube;
        graphics::texture_object red_cube_fade;
        graphics::texture_object red_jump;
        graphics::texture_object red_spike;
        graphics::texture_object red_spike_fade;

        graphics::texture_object neutral_cube;
        graphics::texture_object neutral_jump;
        graphics::texture_object neutral_spike;

        graphics::texture_object background;
        graphics::texture_object player_text;

        void resolve_block_collisions(block* beg, block* end)
        {
            for (; beg != end; ++beg)
            {
                auto old = user.bounding_box();
                if (physics::collides(beg->bound, old))
                {
                    user.add_to_position(physics::mtv(beg->bound, old));

                    auto user_bound = user.bounding_box();

                    if (beg->bound.max.y == user_bound.min.y)
                        user.set_collision(down);
                    else if (beg->bound.min.y == user_bound.max.y)
                        user.set_collision(up);

                    if (beg->bound.max.x == user_bound.min.x)
                        user.set_collision(left);
                    else if (beg->bound.min.x == user_bound.max.x)
                        user.set_collision(right);
                    
                    for (auto* obj : beg->sub_objs)
                    {
                        if (physics::collides(obj->bound, user.bounding_box()))
                        {
                            switch (obj->type)
                            {
                            case block_type::jump_block:
                                if ((obj->facing == up || obj->facing == down) && (user.get_collision(left) || user.get_collision(right)))
                                    user.set_can_wall_jump();
                                break;
                            case block_type::spike:
                                // kill the user if
                                // they didn't collide from the bottom of the spike and
                                // either they're not sneaking, or they are sneaking and the part they hit wasn't the sides
                                if (!user.get_collision(obj->facing) &&
                                    (!user.get_is_sneaking() || user.get_collision(opposite(obj->facing))))
                                {
                                    reset();
                                }
                            }
                        }
                    }
                }
            }
        }

        void resolve_collisions()
        {
            user.reset_flags();

            if (game_state == state::blue)
                resolve_block_collisions(blue_blocks.data(), blue_blocks.data() + blue_blocks.size());
            else
                resolve_block_collisions(red_blocks.data(), red_blocks.data() + red_blocks.size());
            resolve_block_collisions(neutral_blocks.data(), neutral_blocks.data() + neutral_blocks.size());
            resolve_block_collisions(window_blocks, window_blocks + std::size(window_blocks));
        }

        // expands cur and returns iterator to found expansion (finds box in range that is either directly on top or bottom of cur)
        static std::vector<game_obj*>::iterator find_expansion(std::vector<game_obj*>::iterator begin, std::vector<game_obj*>::iterator end, physics::bounding_box& cur)
        {
            for (auto it = begin; it != end; ++it)
            {
                auto &b = **it;
                if (cur.min.x == b.bound.min.x)
                {
                    // if cur is lower than b
                    if (cur.min.y < b.bound.min.y)
                    {
                        if (cur.max.y == b.bound.min.y)
                        {
                            cur.max = b.bound.max;
                            return it;
                        }
                    }
                    // if b is lower than cur
                    else if (cur.min.y > b.bound.min.y)
                    {
                        if (cur.min.y == b.bound.max.y)
                        {
                            cur.min = b.bound.min;
                            return it;
                        }
                    }
                }
            }
            return end;
        }

        // organized game_objs into vertically alligned groups
        static void create_blocks(std::vector<game_obj>& which, std::vector<block>& which_block)
        {
            which_block.clear();

            std::vector<game_obj*> objs;
            objs.reserve(which.size());
            for (auto& obj : which)
                objs.push_back(&obj);

            while(objs.size())
            {
                block cur{objs.back()->bound, {objs.back()}};
                objs.pop_back();
                do
                {
                    std::vector<game_obj*>::iterator found = find_expansion(objs.begin(), objs.end(), cur.bound);
                    if (found == objs.end())
                        break;
                    cur.sub_objs.push_back(*found);
                    objs.erase(found);
                } while (true);
                
                which_block.push_back(std::move(cur));
            }
        }

        static void set_texture_properties(graphics::texture_object *obj, double width, double height)
        {
            static math::vec4 transparent{0.0, 0.0, 00, 0.0};
            static std::array<math::dvec2, 4> pts{{{}, {}, {}, {}}};
            // set to transparent border
            obj->set_texture_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            obj->set_texture_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            obj->set_texture_parameter(GL_TEXTURE_BORDER_COLOR, math::value(transparent));

            // set pts to correspond to correct dimensions
            pts[1][0] = pts[2][0] = width;
            pts[2][1] = pts[3][1] = height;
            obj->attach_shape(pts);

            // set points to correspond to basic texture square in text coords
            pts[1][0] = pts[2][0] = 1;
            pts[2][1] = pts[3][1] = 1;
            obj->attach_texture_shape(pts);
        }

        static constexpr double jump_buffer_countdown_time = .5;
        static constexpr math::dvec2 cube_size = {64.0, 64.0};
        static constexpr math::dvec2 spike_size = {cube_size[0], cube_size[1] / 2};
        // shrink sides of spike bounding box to make them harder to hit from the side
        static constexpr math::dvec2 spike_bound_size = {spike_size[0] - 10, spike_size[1]};
        static constexpr math::dvec2 player_size = {40.0, 40.0};
    public:
        handler(application &a, math::vec<int, 2> window_dimensions) :
            app{a},
            game_state{state::blue},
            initial_position{},
            window_size{window_dimensions},
            user{0, 0, player_size.x, player_size.y},
            jump_buffered_countdown{jump_buffer_countdown_time},
            blue_objs{}, red_objs{}, neutral_objs{},
            blue_blocks{}, red_blocks{}, neutral_blocks{},
            window_blocks{
                // bottom
                {{{-1000, -1000}, {window_size.x + 1000 * 2, 1000}}, {}},
                // right
                {{{window_size.x, -1000}, {1000, window_size.y + 1000 * 2}}, {}},
                // top
                {{{-1000, window_size.y}, {window_size.x + 1000 * 2, 1000}}, {}},
                // left
                {{{-1000, -1000}, {1000, window_size.y + 1000 * 2}}, {}},
            },
            blue_cube{app, "assets/blue_cube.png"},
            blue_cube_fade{app, "assets/blue_cube_fade.png"},
            blue_jump{app, "assets/blue_jump.png"},
            blue_spike{app, "assets/blue_spike.png"},
            blue_spike_fade{app, "assets/blue_spike_fade.png"},
            red_cube{app, "assets/red_cube.png"},
            red_cube_fade{app, "assets/red_cube_fade.png"},
            red_jump{app, "assets/red_jump.png"},
            red_spike{app, "assets/red_spike.png"},
            red_spike_fade{app, "assets/red_spike_fade.png"},
            neutral_cube{app, "assets/neutral_cube.png"},
            neutral_jump{app, "assets/neutral_jump.png"},
            neutral_spike{app, "assets/neutral_spike.png"},
            background{app, "assets/background.png"},
            player_text{app, "assets/player_text.png"}
        {
            set_texture_properties(&blue_cube, cube_size.x, cube_size.y);
            set_texture_properties(&blue_cube_fade, cube_size.x, cube_size.y);
            set_texture_properties(&blue_jump, cube_size.x, cube_size.y);
            set_texture_properties(&blue_spike, spike_size.x, spike_size.y);
            set_texture_properties(&blue_spike_fade, spike_size.x, spike_size.y);

            set_texture_properties(&red_cube, cube_size.x, cube_size.y);
            set_texture_properties(&red_cube_fade, cube_size.x, cube_size.y);
            set_texture_properties(&red_jump, cube_size.x, cube_size.y);
            set_texture_properties(&red_spike, spike_size.x, spike_size.y);
            set_texture_properties(&red_spike_fade, spike_size.x, spike_size.y);

            set_texture_properties(&neutral_cube, cube_size.x, cube_size.y);
            set_texture_properties(&neutral_jump, cube_size.x, cube_size.y);
            set_texture_properties(&neutral_spike, spike_size.x, spike_size.y);

            set_texture_properties(&background, window_size.x, window_size.y);
            set_texture_properties(&player_text, player_size.x, player_size.y);
        }

        bool load_file(std::string_view file_name)
        {
            loader l;

            if (!l.load(file_name))
                return false;

            blue_objs.clear();
            red_objs.clear();
            neutral_objs.clear();

            initial_position = l.get_spawn();

            for (auto b : l.get_objs())
            {
                math::dvec2 texture_size;
                math::dvec2 bound_size;
                math::dvec2 bound_loc;

                if (b.type == block_type::block || b.type == block_type::jump_block)
                {
                    bound_loc = b.min;
                    texture_size = bound_size = cube_size;
                }
                else
                {
                    switch (b.facing)
                    {
                    case up:
                    case down:
                        texture_size = spike_size;
                        bound_size = spike_bound_size;
                        bound_loc = b.min + math::dvec2{5, 0};
                        break;
                    case left:
                    case right:
                        texture_size = {spike_size.y, spike_size.x};
                        bound_size = {spike_bound_size.y, spike_bound_size.x};
                        bound_loc = b.min + math::dvec2{0, 5};
                    }
                }


                switch (b.col)
                {
                case block_color::blue:
                    blue_objs.emplace_back(b.min, texture_size, bound_loc, bound_size, b.type, b.facing);
                    break;
                case block_color::red:
                    red_objs.emplace_back(b.min, texture_size, bound_loc, bound_size, b.type, b.facing);
                    break;
                case block_color::neutral:
                    neutral_objs.emplace_back(b.min, texture_size, bound_loc, bound_size, b.type, b.facing);
                    break;
                }
            }

            create_blocks(blue_objs, blue_blocks);
            create_blocks(red_objs, red_blocks);
            create_blocks(neutral_objs, neutral_blocks);

            reset();

            return true;
        }

        void move_player_right()
        {
            user.move_right();
        }

        void move_player_left()
        {
            user.move_left();
        }

        void stop_player_horizontal()
        {
            user.stop_x();
        }

        void player_jump()
        {
            jump_buffered_countdown.start();
        }

        void flip()
        {
            game_state = static_cast<state>(!static_cast<bool>(game_state));
        }

        void update(double seconds)
        {
            if (!jump_buffered_countdown.finished() && user.jump())
                jump_buffered_countdown.finish();

            user.update(seconds);
            
            resolve_collisions();
        }

        math::dvec2 player_velocity() const
        {
            return user.velocity();
        }

        void reset()
        {
            user = player(initial_position.x, initial_position.y, player_size.x, player_size.y);
        }

        void draw(const math::mat<float, 4, 4>& projection)
        {
            background.draw(math::identity(), math::identity(), projection);

            {
                auto pos = user.position();
                player_text.draw(math::translate<float>(pos.x, pos.y, 0), math::identity(), projection);
            }

            // store corresponding texture object (either faded or not) depending on game state
            graphics::texture_object* blue_cube_ptr;
            graphics::texture_object* blue_jump_ptr;
            graphics::texture_object* blue_spike_ptr;

            graphics::texture_object* red_cube_ptr;
            graphics::texture_object* red_jump_ptr;
            graphics::texture_object* red_spike_ptr;
            switch (game_state)
            {
            case state::blue:
                blue_cube_ptr = &blue_cube;
                blue_jump_ptr = &blue_jump;
                blue_spike_ptr = &blue_spike;

                red_cube_ptr = &red_cube_fade;
                red_jump_ptr = &red_cube_fade;
                red_spike_ptr = &red_spike_fade;
                break;
            case state::red:
                red_cube_ptr = &red_cube;
                red_jump_ptr = &red_jump;
                red_spike_ptr = &red_spike;

                blue_cube_ptr = &blue_cube_fade;
                blue_jump_ptr = &blue_cube_fade;
                blue_spike_ptr = &blue_spike_fade;
            }

            for (auto& obj : blue_objs)
            {
                switch (obj.type)
                {
                case block_type::block:
                    obj.draw(projection, blue_cube_ptr);
                    break;
                case block_type::jump_block:
                    obj.draw(projection, blue_jump_ptr);
                    break;
                case block_type::spike:
                    obj.draw(projection, blue_spike_ptr);
                }
            }

            for (auto& obj : red_objs)
            {
                switch (obj.type)
                {
                case block_type::block:
                    obj.draw(projection, red_cube_ptr);
                    break;
                case block_type::jump_block:
                    obj.draw(projection, red_jump_ptr);
                    break;
                case block_type::spike:
                    obj.draw(projection, red_spike_ptr);
                }
            }

            for (auto& obj : neutral_objs)
            {
                switch (obj.type)
                {
                case block_type::block:
                    obj.draw(projection, &neutral_cube);
                    break;
                case block_type::jump_block:
                    obj.draw(projection, &neutral_jump);
                    break;
                case block_type::spike:
                    obj.draw(projection, &neutral_spike);
                }
            }
        }
    };
}