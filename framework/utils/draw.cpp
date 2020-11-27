#include "draw.hpp"

void draw::init()
{
	fonts::main = csgo::i::surface->FontCreate();
	fonts::esp = csgo::i::surface->FontCreate();
	fonts::smallest = csgo::i::surface->FontCreate();
	fonts::big = csgo::i::surface->FontCreate();

	csgo::i::surface->SetFontGlyphSet(fonts::main, "Verdana", 12, 0, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	csgo::i::surface->SetFontGlyphSet(fonts::esp, "Segoe UI", 13, 500, 0, 0, FONTFLAG_ANTIALIAS);
	//csgo::i::surface->SetFontGlyphSet(fonts::esp, "Segoe UI",	14, 200, 20, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	csgo::i::surface->SetFontGlyphSet(fonts::smallest, "Smallest Pixel-7", 11, 400, 0, 0, FONTFLAG_OUTLINE);
	csgo::i::surface->SetFontGlyphSet(fonts::big, "Verdana", 26, FW_BOLD, 0, 0, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
}

void draw::string(int x, int y, HFont font, Color col, std::string text, int mode) // i tried to be different idk prob very inefficient
{
	std::wstring wideString;
	for (int i = 0; i < text.length(); ++i)
		wideString += wchar_t(text[i]);

	csgo::i::surface->DrawSetTextFont(font);

	vec2_t Pos;

	if (!mode)
		Pos = {float(x), float(y)};
	else
	{
		vec2_t Size = text_size(font, wideString);

		//could make an enum 2 lazy rn tho

		if (mode == 1) // center
			Pos = {float(x - (float(.5 + (Size.x / 2)))), float(y - (float(.5 + (Size.y / 2))) + 1)};
		else if (mode == 2) // x backwards
			Pos = {float(x - Size.x), float(y - (float(.5 + (Size.y / 2))) + 1)};
		else if (mode == 3) // y center only
			Pos = {(float) x, float(y - (float(.5 + (Size.y / 2))) + 1)};
		else if (mode == 4) // x center only
			Pos = {float(x - (float(.5 + (Size.x / 2)))), (float) y};
	}

	csgo::i::surface->DrawSetTextPos(int(Pos.x), int(Pos.y));
	csgo::i::surface->DrawSetTextColor(col);
	csgo::i::surface->DrawPrintText(wideString.c_str(), wcslen(wideString.c_str()));
}
