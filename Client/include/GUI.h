#pragma once

#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_sdl3.h"
#include "../../imgui/backends/imgui_impl_sdlrenderer3.h"

#include "../../imgui/backends/SDL3/SDL.h"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include "../../imgui/backendsSDL3/SDL_opengles2.h"
#else
#include "../../imgui/backends/SDL3/SDL_opengl.h"
#endif

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

enum class screens { Home, Vol, Media, Spotify };

extern int gui_init();

extern ImVec4*		set_colors();
extern ImGuiStyle& set_style();

extern void draw_main(screens current);

extern void draw_home();
extern void draw_killed();
extern void draw_setup();
extern void draw_disconnected_alert();

screens get_default_screen();

#ifdef _DEBUG
void draw_performance();
#endif
