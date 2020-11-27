#include "features.hpp"
#include "../menu/variables.hpp"
#include "../globals.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <locale>
#include <codecvt>
#include <string>
#include <sstream>

bool features::visuals::bound(player_t* player)
{
	vec3_t origin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
	int left, top, right, bottom;

	origin = player->abs_origin();
	min = player->mins() + origin;
	max = player->maxs() + origin;

	vec3_t points[] = {
		vec3_t(min.x, min.y, min.z),
		vec3_t(min.x, max.y, min.z),
		vec3_t(max.x, max.y, min.z),
		vec3_t(max.x, min.y, min.z),
		vec3_t(max.x, max.y, max.z),
		vec3_t(min.x, max.y, max.z),
		vec3_t(min.x, min.y, max.z),
		vec3_t(max.x, min.y, max.z)
	};

	if (!csgo::render::world_to_screen(flb, points[3]) || !csgo::render::world_to_screen(brt, points[5])
		|| !csgo::render::world_to_screen(blb, points[0]) || !csgo::render::world_to_screen(frt, points[4])
		|| !csgo::render::world_to_screen(frb, points[2]) || !csgo::render::world_to_screen(brb, points[1])
		|| !csgo::render::world_to_screen(blt, points[6]) || !csgo::render::world_to_screen(flt, points[7]))
		return false;

	vec3_t arr[] = {flb, brt, blb, frt, frb, brb, blt, flt};

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}
	box.x = (int) left;
	box.y = (int) top;
	box.w = int(right - left);
	box.h = int(bottom - top);
	return true;
}

void features::visuals::watermark() {

}

void features::visuals::flags(player_t* player, int i)
{
	auto flag = [] (player_t* player, std::string n, Color clr)
	{
		draw::string(box.x + box.w + 4, box.y - 4 + offset[player->idx()], draw::fonts::main, clr, n);
		offset[player->idx()] += 10;
	};

	// literally just figured this out by MarkHC | https://www.unknowncheats.me/forum/1393142-post5.html
	player_info_t player_info;
	csgo::i::engine->get_player_info(i, &player_info);
	bool bot = player_info.m_fake_player;

	if (bot)
		flag(player, "bot", Color(255, 255, 255, alpha[player->idx()]));

	if (player->armor() > 0)
	{
		if (player->has_helmet())
			flag(player, "hk", Color(255, 255, 255, alpha[player->idx()]));
		else
			flag(player, "k", Color(255, 255, 255, alpha[player->idx()]));
	}

	flag(player, "$" + std::to_string(player->account()), Color(133, 187, 101, alpha[player->idx()]));

	if (player->scoped())
		flag(player, "zoom", Color(255, 255, 255, alpha[player->idx()]));
}

void features::visuals::name(player_t* player, int i)
{
	Color clr = Color(vars.visuals.name_clr.r, vars.visuals.name_clr.g, vars.visuals.name_clr.b, alpha[player->idx()]);
	player_info_t player_info;
	csgo::i::engine->get_player_info(i, &player_info);
	std::string str = player_info.m_name;
	draw::string(box.x + box.w / 2, box.y - 6, draw::fonts::main, clr, str, true);
}

void features::visuals::hitmarker_event(event_t* event)
{
	if (!vars.visuals.hitsound)
		return;

	int attacker = csgo::i::engine->get_player_for_userid(event->get_int("attacker"));

	if (attacker != csgo::i::engine->get_local_player()) /* checking if the attacker is local so we dont do events for enemy *NIGGERS* - retardation tyler */
		return;

	csgo::i::engine->client_cmd_unrestricted("play buttons\\arena_switch_press_02.wav");
}

void features::visuals::ammo(player_t* player)
{
	Color clr = Color(vars.visuals.ammo_clr.r, vars.visuals.ammo_clr.g, vars.visuals.ammo_clr.b, alpha[player->idx()]);
	auto wpn = player->weapon();

	if (!wpn)
		return;

	auto data = wpn->data();

	if (!data)
		return;

	auto clip = wpn->ammo();
	auto max_clip = data->m_max_clip;

	int delta = box.w * clip / max_clip;

	draw::rectangle(box.x, box.y + box.h + 4, box.w, 3, Color(0, 0, 0, alpha[player->idx()] / 2));
	draw::rectangle(box.x, box.y + box.h + 4, delta, 3, clr);
	draw::outlinedrect(box.x, box.y + box.h + 4, box.w, 3, Color(0, 0, 0, alpha[player->idx()]));
}

void features::visuals::weapon(player_t* player)
{
	Color clr = Color(vars.visuals.wep_clr.r, vars.visuals.wep_clr.g, vars.visuals.wep_clr.b, alpha[player->idx()]);
	auto normal_wpn_str = [] (player_t* player) -> std::string
	{
		auto wpn = player->weapon();

		if (!wpn)
			return " ";

		auto data = wpn->data();

		if (!data)
			return " ";

		std::string m_wpn_name = data->m_weapon_name;
		m_wpn_name.erase(0, 7); /* takes away weapon_ */
		std::transform(m_wpn_name.begin(), m_wpn_name.end(), m_wpn_name.begin(), ::tolower); /* lower looks better tbh i want this cheat to look very original */
		if (m_wpn_name == "knife_t")
			return "knife";
		else if (m_wpn_name == "usp_silencer")
			return "usp s";
		else if (m_wpn_name == "m4a1_silencer")
			return "m4a1 s";
		else
			return m_wpn_name;
	};

	if (vars.visuals.ammo)
		draw::string(box.x + box.w / 2, box.y + box.h + 12, draw::fonts::main, clr, normal_wpn_str(player), true);
	else
		draw::string(box.x + box.w / 2, box.y + box.h + 6, draw::fonts::main, clr, normal_wpn_str(player), true);
}

void features::visuals::health_bar(player_t* player)
{
	int delta = player->health() * box.h / 100;
	draw::rectangle(box.x - 6, box.y, 3, box.h, Color(0, 0, 0, alpha[player->idx()] / 2));
	draw::rectangle(box.x - 6, box.y + (box.h - delta), 3, delta, Color(0, 255, 0, alpha[player->idx()]));
	draw::outlinedrect(box.x - 6, box.y, 3, box.h, Color(0, 0, 0, alpha[player->idx()]));

	if (!(player->health() == 100))
		draw::string(box.x - 6, box.y + (box.h - delta), draw::fonts::smallest, Color(255, 255, 255, alpha[player->idx()]), std::to_string(player->health()), true);
}

void features::visuals::bounding_box(player_t* player)
{
	Color clr = Color(vars.visuals.box_clr.r, vars.visuals.box_clr.g, vars.visuals.box_clr.b, alpha[player->idx()]);
	draw::outlinedrect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, alpha[player->idx()]));
	draw::outlinedrect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0, alpha[player->idx()]));
	draw::outlinedrect(box.x, box.y, box.w, box.h, clr);
}

void features::visuals::run() {
	for (int i = 1; i <= csgo::i::globals->m_max_clients; i++)
	{
		player_t* player = csgo::i::ent_list->get<player_t*>(i);
		player_t* local = csgo::i::ent_list->get<player_t*>(csgo::i::engine->get_local_player());

		if (!player
			|| !player->alive()
			|| player == local
			|| player->team() == local->team())
			continue;

		offset[player->idx()] = 0;
		alpha[player->idx()] = player->dormant() ? max(alpha[player->idx()] - 5, 100) : min(alpha[player->idx()] + 5, 255);

		if (visuals::bound(player))
		{
			if (vars.visuals.name)
				visuals::name(player, i);

			if (vars.visuals.health)
				visuals::health_bar(player);

			if (vars.visuals.box)
				visuals::bounding_box(player);

			if (vars.visuals.weapon)
				visuals::weapon(player);

			if (vars.visuals.flags)
				visuals::flags(player, i);

			if (vars.visuals.ammo)
				visuals::ammo(player);
		}
	}
}
