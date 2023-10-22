#pragma once

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "libcmt.lib")

#pragma comment(lib, "../lib/SDL3/SDL3.lib")
//#pragma comment(lib, "../imgui/lib/FreeType/freetype.lib")

#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_SDL3.h"
#include "../../imgui/backends/imgui_impl_sdlrenderer3.h"
#include "../../imgui/backends/SDL3/SDL.h"

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

int gui_init();

void draw_main();

ImVec4*		set_colors();
ImGuiStyle& set_style();

#ifdef _DEBUG
void draw_performance();
#endif