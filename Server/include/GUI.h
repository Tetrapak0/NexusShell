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

extern bool should_draw_id_properties;
extern bool should_draw_button_properties;
extern bool clear_dialog_shown;

extern int button_properties_to_draw;

extern std::string selected_id;

extern int gui_init();
extern int tray_init();

extern void draw_main();
extern void error_dialog(int type, std::string message);

ImVec4*		set_colors();
ImGuiStyle& set_style();

#ifdef _DEBUG
extern void draw_performance();
#endif
