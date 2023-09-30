#pragma once

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "version.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "libcmt.lib")

#pragma comment(lib, "../imgui/lib/SDL3/SDL3.lib")
#pragma comment(lib, "../imgui/lib/FreeType/freetype.lib")

#include "../../imgui/imgui.h"
#include "../../imgui/backends/imgui_impl_SDL3.h"
#include "../../imgui/backends/imgui_impl_sdlrenderer3.h"
#include "../../imgui/backends/SDL3/SDL.h"

int gui_init();

void draw_main();

ImVec4*		set_colors();
ImGuiStyle& set_style();

#ifdef _DEBUG
void draw_performance();
#endif