#pragma once
#include <sdk.hpp>
#include "../utils/draw.hpp"
#include "../hooks.hpp"
#include "../menu/variables.hpp"
#include <buttons.hpp>
#include "../globals.hpp"
#include <regex>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sys/stat.h>
#include <chrono> 
#include <ctime> 
#include <iostream>
#include "../discord_rpc/rpc.h"

namespace features {

	namespace movement {
		void run(ucmd_t* ucmd);
		void fixmove(ucmd_t* ucmd, vec3_t wish_angles);
	}

	namespace visuals {
		inline int offset[65];
		inline float alpha[65];

		bool bound(player_t* player);

		void flags(player_t* player, int i);

		void watermark();

		void name(player_t* player, int i);

		void hitmarker_event(event_t* event);

		void ammo(player_t* player);

		void weapon(player_t* player);

		void health_bar(player_t* player);

		void bounding_box(player_t* player);

		void run();

		struct
		{
			int x, y, w, h;
		} box;
	}
}
