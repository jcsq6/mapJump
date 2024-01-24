#ifndef UTILITY_H
#define UTILITY_H

class key
{
	bool was_pressed;
	bool pressed;

public:
	key() : was_pressed{false}, pressed{false} {}

	bool is_pressed() const { return pressed; }
	bool is_repeated() const { return was_pressed && pressed; }
	bool is_initial_press() const { return pressed && !was_pressed; }
	void update(bool is_pressed) { was_pressed = pressed; pressed = is_pressed; }
};

#endif