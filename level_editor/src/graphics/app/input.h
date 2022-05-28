#pragma once
#include <map>
#include "vec.h"

GRAPHICS_BEG

class key
{    
    bool was_pressed;
    bool pressed;

    friend class input;

public:
    key() : was_pressed{false}, pressed{false} {}

    bool is_pressed() const
    {
        return pressed;
    }

    bool is_repeated() const
    {
        return was_pressed && pressed;
    }

    bool is_initialPress() const
    {
        return pressed && !was_pressed;
    }
};

class input
{
    mutable std::map<int, key> keys;
    mutable std::map<int, key> buttons;
    math::dvec2 pos;

    input() = default;

public:
    static input *get_instance()
    {
        static input instance;
        return &instance;
    }

    void update();
    void update_wait();

    math::dvec2 mouse_pos() const
    {
        return pos;
    }

    const key &get_key(int key_code) const;
    const key &get_mouse_button(int button_code) const;
};

GRAPHICS_END