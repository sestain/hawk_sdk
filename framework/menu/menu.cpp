#include "menu.hpp"

void setup::draw()
{
	std::vector <const char*> tabs = {
		"Visuals",
		"Players",
		"Settings"
	};

	static bool opened = false;
	static menu::wsize_t size = {595, 418};
	menu::init("Counter-Strike: Global Offensive");
	if (menu::begin_window("sdk", &opened, size)) {
		if (menu::begin_tabs(tabs)) {
			switch (menu::tab_value)
			{
			case 0: /* Visuals */
				if (menu::groupbox("Visuals", 0, 0, menu::size.w / 3.11, menu::size.h / 1.16)) {
					menu::colorpicker("name_clr", vars.visuals.name_clr);
					menu::checkbox("Name", &vars.visuals.name);
					menu::colorpicker("box_clr", vars.visuals.box_clr);
					menu::checkbox("Box", &vars.visuals.box);
					menu::checkbox("Health Bar", &vars.visuals.health);
					menu::colorpicker("wep_clr", vars.visuals.wep_clr);
					menu::checkbox("Weapon", &vars.visuals.weapon);
					menu::colorpicker("ammo_clr", vars.visuals.ammo_clr);
					menu::checkbox("Ammo", &vars.visuals.ammo);
					menu::checkbox("Flags", &vars.visuals.flags);
				}

				if (menu::groupbox("Effects", menu::size.w / 3.11 + 10, 0, menu::size.w / 3.11, menu::size.h / 1.16)) {
					menu::checkbox("Camera FOV", &vars.visuals.camera_fov);
					menu::slider("FOV Range", &vars.visuals.camera_fov_range, 160);
					menu::checkbox("Hitsound", &vars.visuals.hitsound);
				}
				break;
			case 1: /* Players*/
				if (menu::groupbox("Enemy", 0, 0, menu::size.w / 3.11, menu::size.h / 1.16)) {
					menu::colorpicker("glow_clr", vars.players.glow_clr);
					menu::checkbox("Glow", &vars.players.glow);
					if (vars.players.glow)
						menu::slider("Glow Opacity", &vars.players.glow_opacity, 255);
				}

				if (menu::groupbox("Colored Models", menu::size.w / 3.11 + 10, 0, menu::size.w / 3.11, menu::size.h / 1.16)) {
					menu::checkbox("Override Player Model", &vars.players.model_override);
					if (vars.players.model_override) {
						menu::colorpicker("mdl_vis", vars.players.model_vis_clr);
						menu::checkbox("Model Visible", &vars.players.model_visible);

						if (vars.players.model_visible)
							menu::slider("Visible Opacity", &vars.players.model_vis_alpha, 255);

						menu::colorpicker("mdl_xqz", vars.players.model_xqz_clr);
						menu::checkbox("Model XQZ", &vars.players.model_xqz);

						if (vars.players.model_xqz)
							menu::slider("XQZ Opacity", &vars.players.model_xqz_alpha, 255);

						menu::combobox("Material", {"Textured", "Flat"}, &vars.players.mdl);

					}
					break;
			case 2: /* movement */
				if (menu::groupbox("groupbox1", 0, 0, menu::size.w / 3.11, menu::size.h / 1.16)) {
					menu::checkbox("BunnyHop", &vars.movement.bhop);
				}

				if (menu::groupbox("groupbox2", menu::size.w / 3.11 + 10, 0, menu::size.w / 3.11, menu::size.h / 1.16)) {
					menu::colorpicker("menu_accent", menu::accent);
					menu::label("menu theme");
					menu::checkbox("discord rpc", &vars.misc.rpc);
				}
				break;
				}
			}
			menu::end_window();
		}
	}
}
