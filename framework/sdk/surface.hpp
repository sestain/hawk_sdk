#pragma once
#include "../utils/vfunc.hpp"
#include "color.hpp"

#define VF(y, x, z, ...) x y##(__VA_ARGS__)\
	{ \
	    _asm mov eax, dword ptr[ecx]\
	    _asm mov edx, z\
	    _asm imul edx, 4\
	    _asm mov eax, dword ptr[eax + edx]\
	    _asm pop ebp\
	    _asm pop ebp\
	    _asm jmp eax\
	}

typedef unsigned long HFont;

struct Vertex_t
{
	Vertex_t() {}
	Vertex_t(const vec2_t& pos, const vec2_t& coord = vec2_t(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const vec2_t& pos, const vec2_t& coord = vec2_t(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	vec2_t	m_Position;
	vec2_t	m_TexCoord;
};

typedef  Vertex_t FontVertex_t;

enum FontDrawType_t
{
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};

// Refactor these two
struct CharRenderInfo
{
	// In:
	FontDrawType_t	drawType;
	wchar_t			ch;

	// Out
	bool			valid;

	// In/Out (true by default)
	bool			shouldclip;
	// Text pos
	int				x, y;
	// Top left and bottom right
	Vertex_t		verts[2];
	int				textureId;
	int				abcA;
	int				abcB;
	int				abcC;
	int				fontTall;
	HFont		currentFont;
};

enum SurfaceFeature_e
{
	ANTIALIASED_FONTS = 1,
	DROPSHADOW_FONTS = 2,
	ESCAPE_KEY = 3,
	OPENING_NEW_HTML_WINDOWS = 4,
	FRAME_MINIMIZE_MAXIMIZE = 5,
	OUTLINE_FONTS = 6,
	DIRECT_HWND_RENDER = 7,
};

struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};

// adds to the font
enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
	FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
};

class c_surface {
public:
	VF(DrawSetColor, void, 14, Color col);
	VF(DrawSetColor, void, 15, int r, int g, int b, int a);
	VF(DrawFilledRect, void, 16, int x0, int y0, int x1, int y1);
	VF(DrawOutlinedRect, void, 18, int x0, int y0, int x1, int y1);
	VF(DrawLine, void, 19, int x0, int y0, int x1, int y1);
	VF(DrawPolyLine, void, 20, int* x, int* y, int count);
	VF(DrawSetTextFont, void, 23, HFont);
	VF(DrawSetTextColor, void, 24, Color col);
	VF(DrawSetTextColor, void, 25, int r, int g, int b, int a);
	VF(DrawSetTextPos, void, 26, int x, int y);
	VF(DrawPrintText, void, 28, const wchar_t* text, int size);
	VF(DrawPrintText, void, 37, int iTextureID, unsigned char const* col, int width, int height);
	VF(DrawPrintText, void, 38, int iTextureID);
	VF(CreateNewTextureID, int, 43, bool procedural);
	VF(FontCreate, HFont, 71);
	VF(SetFontGlyphSet, bool, 72, HFont font, const char* csFontName, int iHeight, int iWeight, int iBlur, int iScanLinesAmount, int iFlags, int iRangeMinimum = 0, int iRangeMaximum = 0);
	VF(GetTextSize, void, 79, HFont font, const wchar_t* text, int& wide, int& tall);
	VF(DrawOutlinedCircle, void, 103, int x, int y, int radius, int size);
	VF(DrawTexturedPolygon, void, 106, int size, FontVertex_t* vertex, bool clipVertices = true);

	void unlock_cursor(void) {
		using unlockcusor_fn = void(__thiscall*)(void*);
		vfunc< unlockcusor_fn >(this, 66)(this);
	}

	void lock_cursor(void) {
		using lockcusor_fn = void(__thiscall*)(void*);
		vfunc< lockcusor_fn >(this, 67)(this);
	}

	void get_cursor_pos(int& x, int& y) {
		using get_cursor_pos_fn = void(__thiscall*)(void*, int&, int&);
		vfunc< get_cursor_pos_fn >(this, 100)(this, x, y);
	}

	void String(int X, int Y, Color Color, int Font, bool Center, const char* _Input, ...)
	{
		int apple = 0;
		char Buffer[2048] = {'\0'};
		va_list Args;

		va_start(Args, _Input);
		vsprintf_s(Buffer, _Input, Args);
		va_end(Args);

		size_t Size = strlen(Buffer) + 1;
		wchar_t* WideBuffer = new wchar_t[Size];

		mbstowcs_s(0, WideBuffer, Size, Buffer, Size - 1);

		int Width = 0, Height = 0;

		if (Center)
			GetTextSize(Font, WideBuffer, Width, Height);

		DrawSetTextColor(Color.r(), Color.g(), Color.b(), Color.a());
		DrawSetTextFont(Font);
		DrawSetTextPos(X - (Width / 2), Y);
		DrawPrintText(WideBuffer, wcslen(WideBuffer));
	}


	void Line(int x, int y, int x2, int y2, Color color)
	{
		DrawSetColor(color.r(), color.g(), color.b(), color.a());
		DrawLine(x, y, x2, y2);
	}

	void FilledRect(int x, int y, int w, int h, Color color)
	{
		DrawSetColor(color.r(), color.g(), color.b(), color.a());
		DrawFilledRect(x, y, x + w, y + h);
	}

	void OutlinedRect(int x, int y, int w, int h, Color color)
	{
		DrawSetColor(color.r(), color.g(), color.b(), color.a());
		DrawOutlinedRect(x, y, x + w, y + h);
	}

	void OutlinedVecRect(int x, int y, int x2, int y2, Color color)
	{
		DrawSetColor(color.r(), color.g(), color.b(), color.a());
		DrawLine(x, y, x2, y);
		DrawLine(x2, y, x2, y2);
		DrawLine(x2, y2, x, y2);
		DrawLine(x, y2, x, y);
	}

};
