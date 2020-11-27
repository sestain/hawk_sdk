#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include <cstdlib>
#include <memory>
#include <chrono>
#include <string>
#include "../renderer/d3d9.hpp"

namespace menu {
	inline LPCSTR window_game_name;
	inline int tab_value;
	inline bool prev_keystate[256];
	inline bool keystate[256];

	struct color {
		int r, g, b, a;
	};

	inline color accent = {
	  5,
	  105,
	  255,
	  255
	};

	class vector {
	public:
		float x, y;
	};

	class csize_t {
	public:
		int x, y, w, h;
	};

	class wsize_t {
	public:
		int w, h;
	};

	struct keybind_t
	{
		int key = -1;
		int type = 0;
	};

	enum class controls : int {
		control_checkbox,
		control_slider,
		control_combobox,
		control_colorpicker,
		control_keybind,
	};

	struct info_t {
		bool opened;
		int placeholder_width = 60;
		int last_group_width;
		const char* item_name;
		controls clicked_control;

		struct {
			std::vector < std::string > items;
			vector clicked_position;
			bool opened;
			int variable;
			int saved_combo_width;
		}
		combo;

		struct {
			vector clicked_position;
			bool opened;
			int saved_combo_width;
			bool copied, pasted;
			bool options_opened;
			std::vector < std::string > option_items = {
			  "Copy",
			  "Paste",
			  "Reset"
			};
			int variable = -1;
		}
		colorpicker;

		struct {
			bool opened;
			bool options_opened;
			std::vector < std::string > option_items = {
			  "Always Off",
			  "On Toggle",
			  "On Hold",
			  "Always on"
			};
			int type_selected;
		}
		keybind;
	};
	inline info_t info;

	inline vector offset;
	static vector pos = {
	  300,
	  300
	};
	inline wsize_t size;
	static menu::color copied_color;
	void spectrum(int x, int y, int w, int h);
	menu::color pen(int x, int y, int w, int h, menu::vector stx);
	void push_focus();
	void init(LPCSTR window_name);
	void end_window();
	bool begin_tabs(std::vector <
		const char* > names);
	void colorpicker(const char* id, menu::color& v);
	bool key_active(menu::keybind_t key);
	void keybind(const char* name, menu::keybind_t* v);
	void checkbox(const char* name, bool* v);
	void label(const char* name);
	bool button(const char* name);
	void combobox(const char* name, std::vector < std::string > items, int* v);
	void slider(const char* name, int* v, int max);
	bool groupbox(const char* name, int x, int y, int w, int h);
	bool is_opened();
	bool begin_window(const char* name, bool* opened, wsize_t& size);

	inline uint32_t twindowgradient = rgba(71, 69, 79, 255);
	inline uint32_t bwindowgradient = rgba(59, 53, 62, 255);
	inline uint32_t background_color = rgba(228, 229, 239, 255);
	inline uint32_t tab_background = rgba(255, 255, 255, 255);

	inline uint32_t top_tab_grad = rgba(88, 84, 99, 255);
	inline uint32_t bottom_tab_grad = rgba(36, 34, 43, 255);

	namespace fonts {
		inline ID3DXFont* fm = nullptr;
		inline ID3DXFont* logo = nullptr;
		inline void init() {
			D3DXCreateFont(csgo::i::dev, 13, 0, FW_LIGHT, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 0, DEFAULT_PITCH | FF_DONTCARE, "Verdana", &fm);
			D3DXCreateFont(csgo::i::dev, 13, 0, FW_LIGHT, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, 0, DEFAULT_PITCH | FF_DONTCARE, "icomoon", &logo);
		}
	}

	namespace input {
		static vector mouse_position() {
			POINT POS;
			GetCursorPos(&POS);
			ScreenToClient(FindWindow(0, window_game_name), &POS);
			return {
			  static_cast <float> (POS.x),
			  static_cast <float> (POS.y)
			};
		}

		static bool in_region(int x, int y, int w, int h) {
			vector mpos = mouse_position();
			if (mpos.x > x && mpos.y > y && mpos.x < x + w && mpos.y < y + h)
				return true;

			return false;
		}

		inline bool key_down(const int vk) {
			return keystate[vk];
		}

		inline bool key_released(const int vk) {
			return !keystate[vk] && prev_keystate[vk];
		}

		inline bool key_clicked(const int vk) {
			return keystate[vk] && !prev_keystate[vk];
		}
	}
}