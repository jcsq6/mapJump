#pragma once
#include "particle.h"
#include "bounding_box.h"
#include "timer.h"
#include "defs.h"

GAME_BEG
class handler;

struct player
{
    physics::bounding_box player_bound;
    physics::particle player_particle;
    double max_air_xvel;

    bool m_wall_jump;
    bool sneaking;

    bool resting;

    utilities::countdown wall_jump_countdown;
    // stores whether there was a collision in the corresponding direction or not
    bool last_collision[4];

    static constexpr double double_jump_wait_time = .5;
    static constexpr double min_maxAirXvel = 150;
    static constexpr double max_xvel = 800;
    static constexpr double ground_velocity_increment_scalar = .1;
    static constexpr double air_velocity_increment_scalar = .02;
    static constexpr double jump_vel = 700;
    static constexpr double gravity = -1800;
    static constexpr double friction = .1;
    static constexpr double sneak_scalar = .1;
    static constexpr double sneak_jump_scalar = .8;

    // returns increment of velocity with respect to the location (air or ground)
    double dxvel()
    {
        return (max_xvel * (sneaking ? sneak_scalar : 1.0)) * (last_collision[down] ? ground_velocity_increment_scalar : air_velocity_increment_scalar);
    }

    double cur_max_xvel()
    {
        return last_collision[down] ? (max_xvel * (sneaking ? sneak_scalar : 1.0)) : max_air_xvel;
    }

public:
    player() = default;

    player(double x, double y, double width, double height) : player_bound(x, y, width, height),
                                                              player_particle{{}, {0, gravity}, {x, y}},
                                                              max_air_xvel{min_maxAirXvel},
                                                              m_wall_jump{false},
                                                              resting{false},
                                                              wall_jump_countdown{double_jump_wait_time}, last_collision{}
    {
    }

    void reset_flags()
    {
        m_wall_jump = false;
        last_collision[left] = last_collision[right] = last_collision[up] = last_collision[down] = false;
    }

    void set_resting()
    {
        resting = !player_particle.vel.x && !player_particle.vel.y;
    }

    void move_right()
    {
        if (!wall_jump_countdown.finished())
            return;

        resting = false;
        player_particle.vel.x += dxvel();
        if (player_particle.vel.x > cur_max_xvel())
            player_particle.vel.x = cur_max_xvel();
    }

    void move_left()
    {
        if (!wall_jump_countdown.finished())
            return;

        resting = false;
        player_particle.vel.x -= dxvel();
        if (player_particle.vel.x < -cur_max_xvel())
            player_particle.vel.x = -cur_max_xvel();
    }

    void stop_x()
    {
        player_particle.vel.x = 0;
    }

    void stop_y()
    {
        player_particle.vel.y = 0;
    }

    void add_to_position(math::dvec2 offset)
    {
        player_particle.pos += offset;
        player_bound.min += offset;
        player_bound.max += offset;
    }

    void set_is_sneaking(bool updated)
    {
        sneaking = updated;
    }

    bool get_is_sneaking() const
    {
        return sneaking;
    }

    physics::bounding_box bounding_box() const
    {
        return player_bound;
    }

    math::dvec2 velocity() const
    {
        return player_particle.vel;
    }

    math::dvec2 position() const
    {
        return player_particle.pos;
    }

    math::dvec2 acceleration() const
    {
        return player_particle.acc;
    }

    bool get_collision(direction dir) const
    {
        return last_collision[dir];
    }

    void set_collision(direction dir)
    {
        last_collision[dir] = true;

        switch (dir)
        {
        case down:
            if (math::sgn(player_particle.vel.y) == -1)
                stop_y();
            break;
        case up:
            if (math::sgn(player_particle.vel.y) == 1)
                stop_y();
            break;
        case left:
            if (math::sgn(player_particle.vel.x) == -1)
                stop_x();
            break;
        case right:
            if (math::sgn(player_particle.vel.x) == 1)
                stop_x();
            break;
        }
    }

    void set_no_collision(direction dir)
    {
        last_collision[dir] = false;
    }

    // make sure to register a collision direction with set_collision
    void set_can_wall_jump()
    {
        m_wall_jump = true;
    }

    // returns true if succsefully jumped
    bool jump()
    {
        if (last_collision[down])
        {
            resting = false;
            player_particle.vel.y = jump_vel * (sneaking ? sneak_jump_scalar : 1);
        }
        else if (m_wall_jump)
        {
            player_particle.vel.y = jump_vel * 1.2;
            // if collided on the left, go right
            player_particle.vel.x = (last_collision[left] ? 1 : -1) * max_air_xvel * .5;

            wall_jump_countdown.start();
        }
        if (last_collision[down] || m_wall_jump)
        {
            max_air_xvel = std::abs(player_particle.vel.x);
            if (max_air_xvel < min_maxAirXvel)
                max_air_xvel = min_maxAirXvel;
            return true;
        }
        return false;
    }

    void update(double seconds)
    {
        if (!resting)
        {
            if (last_collision[down])
            {
                // protect in case they fall off a ledge without jumping, otherwise they'd lose all horizontal velocity
                max_air_xvel = std::abs(player_particle.vel.x);
                if (max_air_xvel < min_maxAirXvel)
                    max_air_xvel = min_maxAirXvel;
                // use friction on ground
                player_particle.update(seconds, friction);
            }
            else
                player_particle.update(seconds, 0);

            player_bound.update_pos(player_particle.pos);
        }
    }
};
GAME_END