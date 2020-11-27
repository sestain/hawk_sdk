#include "rpc.h"
#include <vcruntime_string.h>
#include <regex>
#include "../menu/variables.hpp"
/* CREDITS: WARLOCK https://www.unknowncheats.me/forum/general-programming-and-reversing/361227-adding-discord-rich-presence-cheat.html */

DiscordRichPresence discordPresence;
static bool state = true;
void discord_rpc::init()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize("id here", &handlers, true, nullptr);
	update();
	state = true;
}

void discord_rpc::update()
{
	/* create a discord application @ https://discord.com/developers/applications name it ur hack name
	for Discord_Initialize("id", &handlers, true, nullptr); replace id with your bot client ID.
	for a picture to show add a image inside of Rich Presence  art assets add image than use that name for largeImageKey.
	*/

	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.details = "sdk";
	discordPresence.largeImageKey = ""; //large image file name no extension
	discordPresence.largeImageText = "sdk";
	discordPresence.partySize = /* max size */ 1;
	discordPresence.smallImageKey = ""; //same as large
	discordPresence.smallImageText = ""; //displays on hover
	Discord_UpdatePresence(&discordPresence); //do the do
}

void discord_rpc::shutdown()
{
	if (state)
		Discord_Shutdown();
	state = false;
}
void discord_rpc::disable() {
	Discord_ClearPresence();
}
