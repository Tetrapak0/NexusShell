#pragma once

#ifdef _WIN32
    #pragma comment(linker, "/IMPLIB:NUL /NOIMPLIB /NOEXP")

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    
    #pragma comment(lib, "winmm.lib")
    #pragma comment(lib, "version.lib")
    #pragma comment(lib, "Imm32.lib")
    #pragma comment(lib, "Setupapi.lib")
    #pragma comment(lib, "libcmt.lib")
    
    #pragma comment(lib, "../../external/SDL3/bin/SDL3.lib")
    #pragma comment(lib, "../../external/SDL3/bin/SDL3_image.lib")
    #pragma comment(lib, "../../external/FreeType/bin/freetype.lib")
#endif

#include "../../external/imgui/imgui.h"
#include "../../external/imgui/imgui_stdlib.h"
#include "../../external/imgui/imgui_impl_sdl3.h"
#include "../../external/imgui/imgui_impl_sdlrenderer3.h"
#include "../../external/SDL3/include/SDL_image.h"
#include "../../external/SDL3/include/SDL.h"

#include "../../external/NativeFileDIalogs-Extended/include/nfd.hpp"

//#include "../../external/Roboto/Roboto.h"
#include "../../external/NotoSans/NotoSans.h"

using std::string;

extern bool should_draw_shell_properties;
extern bool should_draw_button_properties;
extern bool clear_dialog_shown;

extern int button_properties_to_draw;

extern string selected_shell;

class Button {
public:
	string label = "";
	string label_backup = label;
	string action;
	bool default_label = true;

	enum class display_types {Text, Image, END};
	display_types display_type = display_types::Text;

	bool generate_thumbnail = false;
	int thumbnail_width, thumbnail_height;
	SDL_Texture* thumbnail = nullptr;

	enum class types {File, URL, Command, Directory, END};
	types type = types::File;
};

enum _gui_flags {
    GF_NONE = 0,        // Hardcoded behaviour (0)
    GF_HIDDEN = 1<<1,   // Hide window on startup (2)
    GF_SOFTWARE = 1<<2, // Software renderer (4)
};

extern int gui_init(int flags);

extern void draw_main();
extern void error_dialog(int type, string message);

extern ImVec4*	   set_colors();
extern ImGuiStyle& set_style();

extern int systray_init(uint32_t flags);

#ifdef _DEBUG
extern void draw_performance();
#endif
