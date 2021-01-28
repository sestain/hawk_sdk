#pragma once
#include <cstdint>
#include <string_view>
#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <iostream>
#include "../sdk/sdk.hpp"

namespace fonts {
	static void* def = nullptr;
}

inline constexpr uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);
}

namespace render {
	extern float frametime;

	struct vtx_t {
		float x, y, z, rhw;
		std::uint32_t color;
	};

	struct custom_vtx_t {
		float x, y, z, rhw;
		std::uint32_t color;
		float tu, tv;
	};

	struct pos {
		int x, y;
	};

	struct dim {
		int w, h;
	};

	struct rect {
		int x, y, w, h;
	};

	//text
	void create_font(void** font, const std::wstring_view& family, int size, bool bold);
	void outline_text(int x, int y, std::uint32_t color, void* font, std::string_view& text);
	void outline_text(pos p, std::uint32_t color, void* font, std::string_view& text);
	void text_size(void* font, std::string_view& text, int& w, int& h);
	void text_size(void* font, std::string_view& text, dim& dimentions);
	void text(int x, int y, std::uint32_t color, void* font, std::string_view& text);
	void text(pos p, std::uint32_t color, void* font, std::string_view& text);

	//rect
	void rectangle(int x, int y, int width, int height, std::uint32_t color);
	void rectangle(rect d, std::uint32_t color);
	void gradient(int x, int y, int width, int height, std::uint32_t color1, std::uint32_t color2, bool is_horizontal);
	void gradient(rect d, std::uint32_t color1, std::uint32_t color2, bool is_horizontal);
	void outline(int x, int y, int width, int height, std::uint32_t color);
	void outline(rect d, std::uint32_t color);
	void clip_rect(int x, int y, int width, int height);
	void clip_rect(rect d);

	//other
	void screen_size(int& width, int& height);
	void screen_size(dim& d);

	void line(int x, int y, int x2, int y2, std::uint32_t color);
	void line(pos p1, pos p2, std::uint32_t color);

	void circle(int x, int y, int radius, int segments, std::uint32_t color);
	void circle(pos p, int radius, int segments, std::uint32_t color);

	void texture(unsigned char* data, int x, int y, int width, int height, float scale);
	
	bool key_pressed(const std::uint32_t key);
}