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
#pragma comment(lib, "../lib/FreeType/freetype.lib")

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_stdlib.h"
#include "../../imgui/backends/imgui_impl_sdl3.h"
#include "../../imgui/backends/imgui_impl_sdlrenderer3.h"
#include "../../imgui/backends/SDL3/SDL.h"

#include "../../NativeFileDIalogs-Extended/include/nfd.hpp"
#include "../../traypp/include/tray.hpp"

#include "../../Helvetica.h"

#define SDL_ERROR(fail_point) {												\
	string what_error(fail_point);											\
	string message = "Failed to initialize ";								\
	message += what_error;													\
	message += SDL_GetError();												\
	const SDL_MessageBoxButtonData button_data[] = {						\
		{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"}					\
	};																		\
	int buttonid;															\
	const SDL_MessageBoxData msgbox_data = {								\
		SDL_MESSAGEBOX_ERROR,												\
		NULL,																\
		"Error!",															\
		message.c_str(),													\
		SDL_arraysize(button_data),											\
		button_data,														\
		NULL																\
	};																		\
	SDL_ShowMessageBox(&msgbox_data, &buttonid);							\
	done = true;															\
	exit(-1);																\
}

extern bool should_draw_properties;
extern bool clear_dialog_shown;

extern int properties_to_draw;
extern int selected_id;

extern std::string selected_id_id;

extern int gui_init();
extern int tray_init();

extern void draw_main();

ImVec4*		set_colors();
ImGuiStyle& set_style();

#ifdef _DEBUG
extern void draw_performance();
#endif
