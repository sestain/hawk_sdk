#pragma once
#include "../sdk/sdk.hpp"
#include "../sdk/surface.hpp"

namespace draw
{
	namespace fonts
	{
		inline HFont main = NULL;
		inline HFont smallest = NULL;
		inline HFont esp = NULL;
		inline HFont big = NULL;
	}

	static void rectangle(int x, int y, int width, int height, Color col)
	{
		csgo::i::surface->DrawSetColor(col);
		csgo::i::surface->DrawFilledRect(x, y, x + width, y + height);
	}

	static void line(int x, int y, int x2, int y2, Color col)
	{
		csgo::i::surface->DrawSetColor(col);
		csgo::i::surface->DrawLine(x, y, x2, y2);
	}

	static void outlinedrect(int x, int y, int width, int height, Color col)
	{
		csgo::i::surface->DrawSetColor(col);
		csgo::i::surface->DrawOutlinedRect(x, y, x + width, y + height);
	}

	static vec2_t text_size(HFont font, std::wstring wtext, std::string text = "")
	{
		std::wstring wideString;

		if (text == "")
			wideString = wtext;
		else
		{
			for (int i = 0; i < text.length(); ++i)
				wideString += wchar_t(text[i]);
		}

		int Width, Height;
		csgo::i::surface->GetTextSize(font, wideString.c_str(), Width, Height);

		return {(float) Width, (float) Height};
	}

	void string(int x, int y, HFont font, Color col, std::string text, int mode = 0);
	void init();
}
