#pragma once
#include "lib/discord_register.h"
#include "lib/discord_rpc.h"
#include "../sdk/sdk.hpp"
// https://www.unknowncheats.me/forum/general-programming-and-reversing/361227-adding-discord-rich-presence-cheat.html

namespace discord_rpc
{
	void init();
	void update();
	void shutdown();
	void disable();
}
