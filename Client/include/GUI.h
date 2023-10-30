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

#define SDL_ERROR(fail_point) { \
cerr << "Failed to create " << fail_point << ": " << SDL_GetError() << "\nPress Enter to exit.";  \
cin.get();																					      \
done = true;																					  \
return -1;																						  \
}

enum class screens { Home, Vol, Media, Spotify };

int gui_init();

ImVec4*		set_colors();
ImGuiStyle& set_style();

void draw_main(screens current);

void countdown_reconnect();

void draw_home();
void draw_loading();
void draw_setup();
void draw_disconnected_alert();

screens get_default_screen();

#ifdef _DEBUG
void draw_performance();
#endif