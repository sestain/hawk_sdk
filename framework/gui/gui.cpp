#include "gui.hpp"

void menu::spectrum(int x, int y, int w, int h) {
	for (int i = 0; i < w; i++) {
		int div = w / 6;
		int phase = i / div;
		float t = (i % div) / (float) div;
		int r, g, b;

		switch (phase) {
		case (0):
			r = 255;
			g = 255 * t;
			b = 0;
			break;
		case (1):
			r = 255 * (1.f - t);
			g = 255;
			b = 0;
			break;
		case (2):
			r = 0;
			g = 255;
			b = 255 * t;
			break;
		case (3):
			r = 0;
			g = 255 * (1.f - t);
			b = 255;
			break;
		case (4):
			r = 255 * t;
			g = 0;
			b = 255;
			break;
		case (5):
			r = 255;
			g = 0;
			b = 255 * (1.f - t);
			break;
		}

		for (int k = 0; k < h; k++) {
			float sat = k / (float) h;
			int _r = r + sat * (128 - r);
			int _g = g + sat * (128 - g);
			int _b = b + sat * (128 - b);

			render::rectangle(x + i, y + k, 1, 1, D3DCOLOR_RGBA(_r, _g, _b, 255));
		}
	}
}

menu::color menu::pen(int x, int y, int w, int h, menu::vector stx) {
	int div = w / 6;
	int phase = stx.x / div;
	float t = ((int) stx.x % div) / (float) div;
	int r, g, b;

	switch (phase) {
	case (0):
		r = 255;
		g = 255 * t;
		b = 0;
		break;
	case (1):
		r = 255 * (1.f - t);
		g = 255;
		b = 0;
		break;
	case (2):
		r = 0;
		g = 255;
		b = 255 * t;
		break;
	case (3):
		r = 0;
		g = 255 * (1.f - t);
		b = 255;
		break;
	case (4):
		r = 255 * t;
		g = 0;
		b = 255;
		break;
	case (5):
		r = 255;
		g = 0;
		b = 255 * (1.f - t);
		break;
	}

	float sat = stx.y / h;
	return {
		static_cast <int> (r + sat * (128 - r)),
		static_cast <int> (g + sat * (128 - g)),
		static_cast <int> (b + sat * (128 - b)),
		255
	};
}

void menu::push_focus() {
	if (info.clicked_control == controls::control_combobox) {
		if (info.combo.opened) {
			auto items = info.combo.items;
			auto v = info.combo.variable;

			for (int i = 0; i < items.size(); i++) {
				csize_t control = {
					info.combo.clicked_position.x,
					info.combo.clicked_position.y + i * 18 + 12,
					info.combo.saved_combo_width,
					18
				};
				render::rectangle(control.x, control.y, control.w, control.h, rgba(44, 41, 52, 255));
				bool in_region = input::in_region(control.x, control.y, control.w, control.h);

				render::outline_text(control.x + 10, control.y + 3, rgba(255, 255, 255, 255), fonts::fm, items[i]);

				if (v == i) {
					render::rectangle(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 35));
					render::outline_text(control.x + 10, control.y + 3, rgba(accent.r, accent.g, accent.b, 255), fonts::fm, items[i]);
				}
				else if (in_region)
					render::rectangle(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 35));
			}
		}
	}

	if (info.clicked_control == controls::control_keybind)
	{
		if (info.keybind.options_opened) {
			auto items = info.keybind.option_items;
			for (int i = 0; i < items.size(); i++) {
				csize_t control = {
					info.colorpicker.clicked_position.x,
					info.colorpicker.clicked_position.y + i * 18 + 12,
					78,
					18
				};
				render::rectangle(control.x, control.y, control.w, control.h, rgba(35, 35, 35, 255));
				bool in_region = input::in_region(control.x, control.y, control.w, control.h);

				render::outline_text(control.x + 10, control.y + 3, rgba(255, 255, 255, 255), fonts::fm, items[i]);

				if (info.keybind.type_selected == i)
				{
					render::rectangle(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 35));
					render::outline_text(control.x + 10, control.y + 3, rgba(accent.r, accent.g, accent.b, 255), fonts::fm, items[i]);
				}
				else if (in_region)
					render::rectangle(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 35));

				if (input::key_down(VK_LBUTTON) && !in_region)
					info.keybind.options_opened = false;
			}
		}
	}

	if (info.clicked_control == controls::control_colorpicker) {
		if (info.colorpicker.opened) {
			vector mouse_position = input::mouse_position();
			static vector moving_position;
			csize_t control = {
				info.colorpicker.clicked_position.x,
				info.colorpicker.clicked_position.y,
				155,
				155
			};
			bool in_region = input::in_region(control.x + 5, control.y + 5, control.w - 10, control.h - 10);
			render::rectangle(control.x, control.y, control.w, control.h, rgba(30, 30, 30, 255));
			render::outline(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 255));
			menu::spectrum(control.x + 5, control.y + 5, control.w - 10, control.h - 10);

			if (input::key_down(VK_LBUTTON) && in_region)
				moving_position = mouse_position;

			render::outline_text(moving_position.x - 4, moving_position.y - 4, rgba(255, 255, 255, 255), fonts::fm, "o");
		}

		if (info.colorpicker.options_opened) {
			auto items = info.colorpicker.option_items;
			int v = info.colorpicker.variable;

			for (int i = 0; i < items.size(); i++) {
				csize_t control = {
					info.colorpicker.clicked_position.x,
					info.colorpicker.clicked_position.y + i * 18 + 12,
					50,
					18
				};
				render::rectangle(control.x, control.y, control.w, control.h, rgba(35, 35, 35, 255));
				bool in_region = input::in_region(control.x, control.y, control.w, control.h);

				render::outline_text(control.x + 10, control.y + 3, rgba(255, 255, 255, 255), fonts::fm, items[i]);

				if (in_region) {
					render::rectangle(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 35));
					render::outline_text(control.x + 10, control.y + 3, rgba(accent.r, accent.g, accent.b, 255), fonts::fm, items[i]);
				}

				if (input::key_down(VK_LBUTTON) && !in_region)
					info.colorpicker.options_opened = false;
			}
		}
	}
}

void menu::init(LPCSTR window_name) {
	menu::window_game_name = window_name;
}

void menu::end_window() {
	if (!info.opened) {
		info.combo.opened = false;
	}

	menu::push_focus();
}

bool menu::begin_tabs(std::vector <
	const char* > names) {
	int add = 47;
	int w = 134, h = 34;
	int add_x = 22;

	for (int i = 0; i < names.size(); i++) {
		render::dim dim;
		render::text_size(fonts::fm, names[i], dim);

		bool in_region = input::in_region(pos.x + add_x, pos.y + add + i * 39, w, h);
		if (menu::tab_value == i)
		{
			render::gradient(pos.x + add_x, pos.y + add + i * 39, w, h, rgba(255, 167, 57, 255), rgba(206, 72, 18, 255), false);
			render::rectangle(pos.x + add_x, pos.y + add + i * 39 + h - 2, w, 2, rgba(0, 0, 0, 130));
		}
		else
		{
			render::gradient(pos.x + add_x, pos.y + add + i * 39, w, h, top_tab_grad, bottom_tab_grad, false);
		}

		render::outline(pos.x + add_x, pos.y + add + i * 39, w, h, rgba(0, 0, 0, 255));

		render::outline_text(pos.x + w / 2 - dim.w / 2 + add_x, pos.y + add + i * 39 + h / 2 - dim.h / 2, rgba(255, 255, 255, 255), fonts::fm, names[i]);

		if (!info.combo.opened && !info.colorpicker.opened && !info.colorpicker.options_opened && !info.keybind.options_opened && input::key_clicked(VK_LBUTTON) && in_region)
			tab_value = i;
	}
	return true;
}

void menu::colorpicker(const char* id, menu::color& v) {
	static int interval = 0;
	csize_t control = {
		offset.x + info.last_group_width - 33,
		offset.y + 1,
		32,
		16
	};
	bool in_region = input::in_region(control.x, control.y, control.w, control.h);

	render::rectangle(control.x, control.y, control.w, control.h, rgba(v.r, v.g, v.b, 255));
	render::outline(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 255));

	if (!info.combo.opened && input::key_clicked(VK_LBUTTON) & in_region) {
		info.item_name = id;
		info.colorpicker.opened = !info.colorpicker.opened;
	}

	if (!info.combo.opened && !info.colorpicker.opened && !info.colorpicker.options_opened && !info.keybind.options_opened && input::key_clicked(VK_RBUTTON) & in_region) {
		info.item_name = id;
		info.colorpicker.options_opened = !info.colorpicker.options_opened;
	}

	if (info.colorpicker.opened) {
		interval = min(interval + 1, 2);
		if (info.item_name == id) {
			info.clicked_control = controls::control_colorpicker;
			info.colorpicker.clicked_position = {
				(float) control.x + 8.f,
				(float) control.y + 8.f
			};
			info.colorpicker.saved_combo_width = info.last_group_width;
			csize_t control = {
				info.colorpicker.clicked_position.x,
				info.colorpicker.clicked_position.y,
				155,
				155
			};
			const vector mouse_position = vector{
				input::mouse_position().x - control.x, input::mouse_position().y - control.y
			};
			if (input::key_down(VK_LBUTTON) && input::in_region(control.x + 5, control.y + 5, 150 - 10, 150 - 10))
				v = menu::pen(control.x, control.y, 150, 150, mouse_position);
		}
	}

	if (info.colorpicker.options_opened && !info.colorpicker.opened) {
		if (info.item_name == id) {
			info.clicked_control = controls::control_colorpicker;
			info.colorpicker.clicked_position = {
				(float) control.x + 8.f,
				(float) control.y + 8.f
			};
			info.colorpicker.saved_combo_width = info.last_group_width;

			auto items = info.colorpicker.option_items;

			for (int i = 0; i < items.size(); i++) {
				csize_t control = {
					info.colorpicker.clicked_position.x,
					info.colorpicker.clicked_position.y + i * 18 + 12,
					50,
					18
				};
				render::rectangle(control.x, control.y, control.w, control.h, rgba(35, 35, 35, 255));
				bool in_region = input::in_region(control.x, control.y, control.w, control.h);

				if (input::key_clicked(VK_LBUTTON) && in_region)
					info.colorpicker.variable = i;

				if (info.colorpicker.variable == 2)
					v = {
					255,
					255,
					255,
					255
				};

				if (info.colorpicker.variable == 0)
					copied_color = v;

				if (info.colorpicker.variable == 1)
					v = copied_color;

				if (info.colorpicker.variable == i)
					info.colorpicker.variable = 0;
			}
		}
	}
}

bool menu::key_active(menu::keybind_t key)
{
	switch (key.type)
	{
	case 0: /* Always off */
		return false;
		break;
	case 1: /* Toggle */
		return GetKeyState(key.key);
		break;
	case 2: /* Hold */
		return GetAsyncKeyState(key.key);
		break;
	case 3: /* Always on */
		return true;
		break;
	}
}

void menu::keybind(const char* name, menu::keybind_t* v) {
	render::dim dim;
	std::string str = std::string(1, char(v->key));

	if (str == std::to_string(NULL))
		v->key = -1;

	if (v->key == -1)
		str = "none";
	else
		str = std::string(1, char(v->key));

	render::text_size(fonts::fm, str, dim);

	csize_t control = {
		offset.x + info.last_group_width - 11 - dim.w / 2,
		offset.y + 1,
		50,
		10
	};
	bool in_region = input::in_region(control.x, control.y, control.w, control.h);

	if (input::key_clicked(VK_LBUTTON) && in_region) {
		info.keybind.opened = !info.keybind.opened;
		info.item_name = name;
	}

	if (input::key_clicked(VK_RBUTTON) && in_region) {
		info.keybind.options_opened = !info.keybind.options_opened;
		info.item_name = name;
	}

	if (info.keybind.options_opened) {
		if (info.item_name == name) {
			info.clicked_control = controls::control_keybind;
			info.keybind.type_selected = v->type;
			info.colorpicker.clicked_position = {
				(float) control.x + 8.f,
				(float) control.y + 8.f
			};

			auto items = info.keybind.option_items;
			for (int i = 0; i < items.size(); i++) {
				csize_t control = {
					info.colorpicker.clicked_position.x,
					info.colorpicker.clicked_position.y + i * 18 + 12,
					70,
					18
				};
				bool in_region = input::in_region(control.x, control.y, control.w, control.h);

				if (input::key_clicked(VK_LBUTTON) && in_region)
					v->type = i;
			}
		}
	}

	if (info.keybind.opened) {
		if (info.item_name == name) {
			info.clicked_control = controls::control_keybind;
			for (int i = 0; i < 255; i++) {
				if (GetAsyncKeyState(i) && !GetAsyncKeyState(VK_LBUTTON) && !GetAsyncKeyState(VK_RBUTTON)) {
					v->key = i;
					if (v->key > 0)
						info.keybind.opened = false;
				}
			}
		}
	}

	render::outline_text(control.x, control.y, info.item_name == name && info.keybind.opened ? rgba(accent.r, accent.g, accent.b, 255) : rgba(80, 80, 80, 255), fonts::fm, info.item_name == name && info.keybind.opened ? "..." : str);
}

void menu::checkbox(const char* name, bool* v) {
	render::dim dim;
	render::text_size(fonts::fm, name, dim);
	csize_t control = {
		offset.x,
		offset.y,
		15,
		15
	};
	bool in_region = input::in_region(control.x, control.y, control.w + dim.w + 12, control.h);

	if (!info.combo.opened && !info.colorpicker.opened && !info.colorpicker.options_opened && !info.keybind.options_opened && input::key_clicked(VK_LBUTTON) && in_region)
		*
		v = !*v;

	render::rectangle(control.x, control.y, control.w, control.h, rgba(44, 41, 52, 255));

	if (*v)
	{
		render::rectangle(control.x + 2, control.y + 2, control.w - 4, control.h - 4, rgba(accent.r, accent.g, accent.b, 255));
		render::outline(control.x + 2, control.y + 2, control.w - 4, control.h - 4, rgba(0, 0, 0, 255));
	}

	render::outline(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 255));
	render::outline_text(control.x + 24, control.y, rgba(255, 255, 255, 255), fonts::fm, name);

	offset.y += control.h + 7;
}

void menu::label(const char* name) {
	csize_t control = {
		offset.x,
		offset.y,
		10,
		10
	};
	render::outline_text(control.x, control.y, rgba(255, 255, 255, 255), fonts::fm, name);

	offset.y += control.h + 12;
}

bool menu::button(const char* name) {
	render::dim dim;
	render::text_size(fonts::fm, name, dim);
	static int interval = 0;
	csize_t control = {
		offset.x,
		offset.y,
		info.last_group_width,
		18
	};
	bool in_region = input::in_region(control.x, control.y, control.w, control.h);

	if (input::key_down(VK_LBUTTON) && in_region)
		render::gradient(control.x, control.y, control.w, control.h, rgba(25, 25, 25, 255), rgba(10, 10, 10, 255), false);
	else
		render::gradient(control.x, control.y, control.w, control.h, top_tab_grad, bottom_tab_grad, false);

	render::outline(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 255));
	render::outline_text(control.x + control.w / 2 - dim.w / 2, control.y + control.h / 2 - dim.h / 2, rgba(255, 255, 255, 255), fonts::fm, name);

	if (input::key_clicked(VK_LBUTTON) && in_region) {
		offset.y += control.h + 7;
		return true;
	}

	offset.y += control.h + 7;
	return false;
}

void menu::combobox(const char* name, std::vector < std::string > items, int* v) {
	static int interval = 0;
	csize_t control = {
		offset.x,
		offset.y + 12,
		info.last_group_width,
		18
	};
	bool in_region = input::in_region(control.x, control.y, control.w, control.h);

	auto arrow = [] (int x, int y, uint32_t clr) -> void {
		for (int i = 0; i < 3; i++)
			render::rectangle(x + i, y + i, 5 - i * 2, 1, clr);
	};

	if (!info.combo.opened && !info.colorpicker.opened && !info.colorpicker.options_opened && !info.keybind.options_opened && input::key_clicked(VK_LBUTTON) && in_region) {
		info.item_name = name;
		info.combo.opened = !info.combo.opened;
	}

	render::rectangle(control.x, control.y, control.w, control.h, rgba(44, 41, 52, 255));
	render::outline(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 255));
	render::outline_text(control.x, control.y - 14, rgba(255, 255, 255, 255), fonts::fm, name);
	arrow(control.x + control.w - 11, control.y + control.h / 2 - 1, rgba(255, 255, 255, 255));
	render::outline_text(control.x + 10, control.y + 3, rgba(255, 255, 255, 255), fonts::fm, items.at(*v));

	if (input::key_clicked(VK_LBUTTON) && !in_region && !info.combo.opened)
		info.combo.opened = false;

	if (info.combo.opened) {
		interval = min(interval + 1, 2);
		if (info.item_name == name) {
			info.clicked_control = controls::control_combobox;
			info.combo.clicked_position = {
				(float) control.x,
				(float) control.y + 12.f
			};
			info.combo.items = items;
			info.combo.variable = *v;
			info.combo.saved_combo_width = info.last_group_width;

			auto items = info.combo.items;

			for (int i = 0; i < items.size(); i++) {
				csize_t control_2 = {
					info.combo.clicked_position.x,
					info.combo.clicked_position.y + i * 18 + 12,
					info.combo.saved_combo_width,
					18
				};
				render::rectangle(control_2.x, control_2.y, control_2.w, control_2.h, rgba(44, 41, 52, 255));
				bool in_region_2 = input::in_region(control_2.x, control_2.y, control_2.w, control_2.h);

				if (input::key_clicked(VK_LBUTTON) && in_region_2) {
					*v = i;
				}

				if (interval == 2) {
					if (input::key_clicked(VK_LBUTTON) && !in_region_2) {
						interval = 0;
						info.combo.opened = false;
					}
				}
			}
		}
	}

	offset.y += control.h + 17;
}

void menu::slider(const char* name, int* v, int max) {
	csize_t control = {
		offset.x,
		offset.y + 12,
		info.last_group_width,
		10
	};
	float pixel = max / (float) control.w;
	bool in_region = input::in_region(control.x, control.y, control.w + 5, control.h);

	if (*v > max)
		*
		v = max;
	else if (*v < 0)
		*
		v = 0;

	render::rectangle(control.x, control.y, control.w, control.h, rgba(44, 41, 52, 255));

	if (!info.combo.opened && !info.colorpicker.opened && !info.colorpicker.options_opened && !info.keybind.options_opened && input::key_down(VK_LBUTTON) && in_region)
		*
		v = (input::mouse_position().x - (control.x)) * pixel;

	render::rectangle(control.x, control.y, (*v / pixel), control.h, rgba(accent.r, accent.g, accent.b, 255));

	render::outline(control.x, control.y, control.w, control.h, rgba(0, 0, 0, 255));
	render::outline_text(control.x, control.y - 14, rgba(255, 255, 255, 255), fonts::fm, name);
	if (*v > 0) {
		render::outline_text(offset.x + (*v / pixel) - 1, offset.y + 14 - 1, D3DCOLOR_RGBA(0, 0, 0, 255), fonts::fm, std::to_string(*v));
		render::outline_text(offset.x + (*v / pixel), offset.y + 14, D3DCOLOR_RGBA(255, 255, 255, 255), fonts::fm, std::to_string(*v));
	}

	offset.y += control.h + 15;
}

bool menu::groupbox(const char* name, int x, int y, int w, int h) {
	render::dim dim;
	render::text_size(fonts::fm, name, dim);

	render::rectangle(pos.x + x + 160 + 15, pos.y + y + 25 + 15, w, h, tab_background);

	render::gradient(pos.x + x + 160 + 15, pos.y + y + 25 + 15, w, 22, rgba(88, 84, 99, 255), rgba(36, 34, 43, 255), false);
	render::outline(pos.x + x + 160 + 15, pos.y + y + 25 + 15, w, 22, rgba(0, 0, 0, 255));

	render::outline(pos.x + x + 160 + 15, pos.y + y + 25 + 15, w, h, rgba(0, 0, 0, 255));
	render::outline_text(pos.x + x + 160 + 15 + 10, pos.y + y + 25 + 15 + 3, rgba(255, 255, 255, 255), fonts::fm, name);

	offset.x = pos.x + x + 160 + 24, offset.y = pos.y + y + 25 + 15 + 28;
	info.last_group_width = w - 17;

	return true;
}

bool menu::is_opened() {
	return info.opened;
}

bool menu::begin_window(const char* name, bool* opened, wsize_t& size) {
	info.opened = *opened;
	menu::size = size;
	offset = {
		0,
		0
	};

	for (int i = 0; i < 256; i++) {
		prev_keystate[i] = keystate[i];
		keystate[i] = GetAsyncKeyState(i);
	}

	if (input::key_clicked(VK_INSERT))
		*opened = !*opened;

	if (!*opened)
		return false;

	static bool dragging_corner = false;
	if (!info.combo.opened && !info.colorpicker.opened) {
		static vector drag = {
			300,
			300
		};

		static vector drag_resize = {
			300,
			300
		};

		bool in_region = input::in_region(pos.x, pos.y, size.w, 28);
		bool in_region_corner = input::in_region(pos.x + size.w - 10, pos.y + size.h - 10, 10, 10);
		static bool dragging = false;

		/* window dragging */
		if (input::key_clicked(VK_LBUTTON) && in_region) {
			drag.x = input::mouse_position().x - pos.x;
			drag.y = input::mouse_position().y - pos.y;
		}

		if (input::key_down(VK_LBUTTON) && in_region)
			dragging = true;

		if (!input::key_down(VK_LBUTTON) && dragging)
			dragging = false;

		if (dragging) {
			pos.x = input::mouse_position().x - drag.x;
			pos.y = input::mouse_position().y - drag.y;
		}

		/* resize dragging */
		if (input::key_clicked(VK_LBUTTON) && in_region_corner) {
			drag_resize.x = input::mouse_position().x - size.w;
			drag_resize.y = input::mouse_position().y - size.h;
		}

		if (input::key_down(VK_LBUTTON) && in_region_corner)
			dragging_corner = true;

		if (!input::key_down(VK_LBUTTON) && dragging_corner)
			dragging_corner = false;

		if (dragging_corner) {
			size.w = input::mouse_position().x - drag_resize.x;
			size.h = input::mouse_position().y - drag_resize.y;
		}
	}

	/* clamp resizing */
	if (size.w <= 580)
		size.w = 580;

	if (size.h <= 382)
		size.h = 382;

	auto draggable = [] (int x, int y, uint32_t clr) -> void
	{
		for (int i = 0; i < 8; i++)
			render::rectangle(x - i, y + i, i, 1, clr);
	};

	render::gradient(pos.x, pos.y, size.w, 25, twindowgradient, bwindowgradient, false);
	render::rectangle(pos.x, pos.y + 25, size.w, size.h - 25, bwindowgradient);
	render::rectangle(pos.x + 5, pos.y + 25, size.w - 10, size.h - 30, background_color);

	draggable(pos.x + size.w - 5, pos.y + size.h - 13, dragging_corner ? rgba(133, 0, 0, 255) : rgba(60, 60, 60, 255));

	render::rectangle(pos.x + 15, pos.y + 25 + 15, 148, size.h / 1.16, tab_background);
	render::outline(pos.x, pos.y, size.w, size.h, rgba(0, 0, 0, 255));
	render::outline(pos.x + 5, pos.y + 25, size.w - 10, size.h - 30, rgba(0, 0, 0, 255));
	render::outline(pos.x + 15, pos.y + 25 + 15, 148, size.h / 1.16, rgba(0, 0, 0, 255));

	//render::outline_text(pos.x + 7, pos.y + 5, rgba(accent.r, accent.g, accent.b, 255), fonts::logo, "b");
	render::outline_text(pos.x + 7, pos.y + 5, rgba(255, 255, 255, 255), fonts::fm, name);

	return true;
}
