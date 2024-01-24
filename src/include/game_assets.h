#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H
#include "arial.h"
#include "background.h"
#include "blue_cube.h"
#include "blue_cube_fade.h"
#include "blue_jump.h"
#include "blue_spike.h"
#include "blue_spike_fade.h"
#include "end_anchor.h"
#include "neutral_cube.h"
#include "neutral_jump.h"
#include "neutral_spike.h"
#include "player_text.h"
#include "red_cube.h"
#include "red_cube_fade.h"
#include "red_jump.h"
#include "red_spike.h"
#include "red_spike_fade.h"
#include "spawn_anchor.h"

#include "gl_instance.h"

struct game_assets
{
	texture background;
	texture blue_cube;
	texture blue_cube_fade;
	texture blue_jump;
	texture blue_spike;
	texture blue_spike_fade;
	texture end_anchor;
	texture neutral_cube;
	texture neutral_jump;
	texture neutral_spike;
	texture player_text;
	texture red_cube;
	texture red_cube_fade;
	texture red_jump;
	texture red_spike;
	texture red_spike_fade;
	texture spawn_anchor;

	static const gl_controlled_data<game_assets> &instance()
	{
		static auto &ga = construct_and_attach<game_assets>();
		return ga;
	}

private:
	template <typename T, typename... Args>
	friend T construct_fun(Args&&... args);

	game_assets() :
		background(GL_RGBA, background_data, background_width, background_height, background_channels),
		blue_cube(GL_RGBA, blue_cube_data, blue_cube_width, blue_cube_height, blue_cube_channels),
		blue_cube_fade(GL_RGBA, blue_cube_fade_data, blue_cube_fade_width, blue_cube_fade_height, blue_cube_fade_channels),
		blue_jump(GL_RGBA, blue_jump_data, blue_jump_width, blue_jump_height, blue_jump_channels),
		blue_spike(GL_RGBA, blue_spike_data, blue_spike_width, blue_spike_height, blue_spike_channels),
		blue_spike_fade(GL_RGBA, blue_spike_fade_data, blue_spike_fade_width, blue_spike_fade_height, blue_spike_fade_channels),
		end_anchor(GL_RGB, end_anchor_data, end_anchor_width, end_anchor_height, end_anchor_channels),
		neutral_cube(GL_RGBA, neutral_cube_data, neutral_cube_width, neutral_cube_height, neutral_cube_channels),
		neutral_jump(GL_RGBA, neutral_jump_data, neutral_jump_width, neutral_jump_height, neutral_jump_channels),
		neutral_spike(GL_RGBA, neutral_spike_data, neutral_spike_width, neutral_spike_height, neutral_spike_channels),
		player_text(GL_RGBA, player_text_data, player_text_width, player_text_height, player_text_channels),
		red_cube(GL_RGB, red_cube_data, red_cube_width, red_cube_height, red_cube_channels),
		red_cube_fade(GL_RGBA, red_cube_fade_data, red_cube_fade_width, red_cube_fade_height, red_cube_fade_channels),
		red_jump(GL_RGBA, red_jump_data, red_jump_width, red_jump_height, red_jump_channels),
		red_spike(GL_RGBA, red_spike_data, red_spike_width, red_spike_height, red_spike_channels),
		red_spike_fade(GL_RGBA, red_spike_fade_data, red_spike_fade_width, red_spike_fade_height, red_spike_fade_channels),
		spawn_anchor(GL_RGBA, spawn_anchor_data, spawn_anchor_width, spawn_anchor_height, spawn_anchor_channels)
	{
	}
};
#endif