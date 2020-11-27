#pragma once
#include "variables.hpp"
#include "../gui/gui.hpp"

struct settings
{

	struct
	{
		menu::color name_clr = {255, 255, 255, 255};
		bool name;
		menu::color wep_clr = {255, 255, 255, 255};
		bool weapon;
		bool health;
		menu::color ammo_clr = {255, 255, 0, 255};
		bool ammo;
		bool flags;
		menu::color box_clr = {255, 255, 255, 255};
		bool box;

		bool camera_fov;
		int camera_fov_range = 90;
		bool hitsound = false;
	} visuals;

	struct
	{
		bool model_override;

		bool model_visible;
		bool model_xqz;

		menu::color model_vis_clr = {112, 169, 255, 255};
		menu::color model_xqz_clr = {255, 112, 155, 255};

		int model_vis_alpha = 255;
		int model_xqz_alpha = 255;

		bool glow;
		menu::color glow_clr = {112, 176, 255, 255};
		int glow_opacity = 140;

		int mdl = 0;
	} players;

	struct
	{
		bool bhop = false;

	} movement;

	struct
	{
		bool rpc = false;

	} misc;

}; extern settings vars;
