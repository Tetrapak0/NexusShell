#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "libcmt.lib")

#pragma comment(lib, "../lib/SDL3/SDL3.lib")
//#pragma comment(lib, "../lib/SDL3/SDL3_image.lib")
#pragma comment(lib, "../lib/FreeType/freetype.lib")

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_stdlib.h"
#include "../../external/imgui/imgui_impl_sdl3.h"
#include "../../external/imgui/imgui_impl_sdlrenderer3.h"
//#include "../../external/SDL3/SDL_image.h"
#include "../../external/SDL3/SDL.h"

#include "../../external/NativeFileDIalogs-Extended/include/nfd.hpp"
#include "../../external/traypp/include/tray.hpp"

#include "../../external/Helvetica.h"

extern bool should_draw_id_properties;
extern bool should_draw_button_properties;
extern bool clear_dialog_shown;
extern bool have_render;

extern int button_properties_to_draw;

extern std::string selected_id;

class button {
public:
	string label = "";
	string label_backup = label;
	string action;
	bool default_label = true;

	enum class display_types { Text, Image, END };
	display_types display_type = display_types::Text;

	bool generate_thumbnail = false;
	int thumbnail_width, thumbnail_height;
	SDL_Texture* thumbnail = nullptr;

	enum class types { File, URL, Command, Directory, END };
	types type = types::File;
};

extern int gui_init();
extern int tray_init();

extern void draw_main();
extern void error_dialog(int type, std::string message);

extern ImVec4*		set_colors();
extern ImGuiStyle& set_style();

#ifdef _DEBUG
extern void draw_performance();
#endif
