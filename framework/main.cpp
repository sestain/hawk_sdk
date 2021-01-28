/* c++ includes */
#include <Windows.h>
#include <thread>
#include "sdk/string_obfuscation.h"
#include <cstdint>
#include <iostream>
#include <array>

/*Hack Includes*/
#include "menu/variables.hpp"
#include "utils/utils.hpp"
#include "minhook/minhook.h"
#include "sdk/sdk.hpp"
#include "hooks.hpp"
#include "globals.hpp"

using namespace std::chrono_literals;

void init(HMODULE mod) {
	/* wait for all modules to load */
	while (!GetModuleHandleA("serverbrowser.dll"))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	/* initialize hack */
	csgo::init();
	netvars::init();
	hooks::init();

	/* wait for unload key */
	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

	/* unload */
	SetWindowLongA(FindWindowA("Valve001", nullptr), GWLP_WNDPROC, long(g_wndproc));

	csgo::i::surface->unlock_cursor();

	FreeLibraryAndExitThread(mod, 0);
}

int __stdcall DllMain(HINSTANCE inst, std::uint32_t reason, void* reserved) {
	if (reason == DLL_PROCESS_ATTACH)
		CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(init), HMODULE(inst), 0, nullptr);

	if (reason == DLL_PROCESS_DETACH)
	{
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}

	return 1;
}
