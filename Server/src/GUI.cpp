#include "../include/Header.hpp"
#include "../include/GUI.hpp"
#include "../include/Config.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb/stb_image.h"

bool done                          = false;
bool should_draw_button_properties = false;
bool should_draw_shell_properties  = false;
bool should_draw_settings          = false;
bool set_to_null                   = true;
bool clear_dialog_shown            = false;

int button_properties_to_draw = -1;

SDL_Renderer* renderer = nullptr;

string selected_shell = "";

ImGuiWindowFlags im_window_flags = ImGuiWindowFlags_NoDecoration
                                 | ImGuiWindowFlags_NoBringToFrontOnFocus
                                 | ImGuiWindowFlags_AlwaysAutoResize
                                 | ImGuiWindowFlags_NoResize
                                 | ImGuiWindowFlags_NoMove;

void gui_get_version() {
    IMGUI_CHECKVERSION();
    SDL_version ver;
    SDL_VERSION(&ver);
    printf("Compiled with Dear ImGui ver. %d and SDL ver. %d.%d.%d\n", 
           IMGUI_VERSION_NUM, ver.major, ver.minor, ver.patch);
}

static ImWchar* gui_get_glyph_ranges() {
    ImFontAtlas ifa;
    int lat = 0;
    int cns = 0;
    int cnf = 0;
    int cyr = 0;
    int grk = 0;
    int jap = 0;
    int kor = 0;
    int tha = 0;
    int vnm = 0;
    static const ImWchar* glyph_range_latin                 = ifa.GetGlyphRangesDefault(&lat);
    static const ImWchar* glyph_range_chinese_simple_common = ifa.GetGlyphRangesChineseSimplifiedCommon(&cns);
    static const ImWchar* glyph_range_chinese_full          = ifa.GetGlyphRangesChineseFull(&cnf);
    static const ImWchar* glyph_range_cyrillic              = ifa.GetGlyphRangesCyrillic(&cyr);
    static const ImWchar* glyph_range_greek                 = ifa.GetGlyphRangesGreek(&grk);
    static const ImWchar* glyph_range_japanese              = ifa.GetGlyphRangesJapanese(&jap);
    static const ImWchar* glyph_range_korean                = ifa.GetGlyphRangesKorean(&kor);
    static const ImWchar* glyph_range_thai                  = ifa.GetGlyphRangesThai(&tha);
    static const ImWchar* glyph_range_vietnamese            = ifa.GetGlyphRangesVietnamese(&vnm);
    ImWchar* glyph_range_full = new ImWchar[lat+cns+cnf+cyr+grk+jap+kor+tha+vnm];
    for (int i = 0; i < lat; ++i) glyph_range_full[i] = glyph_range_latin[i];
    for (int i = 0; i < cns; ++i) glyph_range_full[lat+i] = glyph_range_chinese_simple_common[i];
    for (int i = 0; i < cnf; ++i) glyph_range_full[lat+cns+i] = glyph_range_chinese_full[i];
    for (int i = 0; i < cyr; ++i) glyph_range_full[lat+cns+cnf+i] = glyph_range_cyrillic[i];
    for (int i = 0; i < grk; ++i) glyph_range_full[lat+cns+cnf+cyr+i] = glyph_range_greek[i];
    for (int i = 0; i < jap; ++i) glyph_range_full[lat+cns+cnf+cyr+grk+i] = glyph_range_japanese[i];
    for (int i = 0; i < kor; ++i) glyph_range_full[lat+cns+cnf+cyr+grk+jap+i] = glyph_range_korean[i];
    for (int i = 0; i < tha; ++i) glyph_range_full[lat+cns+cnf+cyr+grk+jap+kor+i] = glyph_range_thai[i];
    for (int i = 0; i < vnm; ++i) glyph_range_full[lat+cns+cnf+cyr+grk+jap+kor+tha+i] = glyph_range_vietnamese[i];
    return glyph_range_full;
}

int gui_init(int flags) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_Window* window = SDL_CreateWindow("NexusShell", 1280, 720, SDL_WINDOW_OPENGL    |
                                                                   SDL_WINDOW_RESIZABLE |
                                                                   SDL_WINDOW_HIDDEN   );
    if (!window) {
        error_dialog(0, "Failed to initialize window.\n");
        SDL_Quit();
        return -1;
    }
    SDL_SetWindowMinimumSize(window, 854, 480);
    uint32_t renderer_flags = SDL_RENDERER_PRESENTVSYNC;
    if (flags & GF_SOFTWARE) renderer_flags |= SDL_RENDERER_SOFTWARE;
    renderer = SDL_CreateRenderer(window, NULL, renderer_flags);
    if (!renderer) {
        error_dialog(0, "Failed to initialize renderer.\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    if (!(flags & GF_HIDDEN)) SDL_ShowWindow(window);
    gui_get_version();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable imgui.ini
    io.LogFilename = nullptr; //
    io.ConfigFlags = ImGuiConfigFlags_EnablePowerSavingMode;

    ImGui::StyleColorsDark(); 
    ImVec4*     colors = set_colors();
    ImGuiStyle& style  = set_style();

    ImWchar* glyph_ranges = gui_get_glyph_ranges();
    ImFontAtlas ifa;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(NotoSans_compressed_data_base85, 20, NULL, glyph_ranges);

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    do {
        ImGui_ImplSDL3_WaitForEvent();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window))) {
                SDL_HideWindow(window);
            }
        }
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        draw_main();
        
#ifdef _DEBUG
        draw_performance();
#endif
        ImGui::Render();
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    } while (!done);
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    delete[] glyph_ranges;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

ImVec4* set_colors() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                 = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_PopupBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]               = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Separator]            = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_SeparatorActive]      = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_ResizeGrip]           = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
    colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                  = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]           = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TabActive]            = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]         = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]   = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]       = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);

    return colors;
}
// TODO: custom style
ImGuiStyle& set_style() {
    ImGuiStyle& style       = ImGui::GetStyle();
    style.ItemSpacing       = ImVec2(0.0f, 0.0f);
    style.ItemInnerSpacing  = ImVec2(0.0f, 0.0f);
    style.GrabMinSize       = 10;
    style.WindowBorderSize  = 0;
    style.ChildBorderSize   = 0;
    style.PopupBorderSize   = 0;
    style.FrameBorderSize   = 0;
    style.TabBorderSize     = 0;
    style.LogSliderDeadzone = 4;

    return style;
}
////// tray.cpp //////
UINT WM_SYSTRAY_CREATED;
NOTIFYICONDATAA notificon_data;

void SystrayCreate() {
    memset(&notificon_data, 0, sizeof(NOTIFYICONDATAA));
    notificon_data.uVersion = NOTIFYICON_VERSION_4;
    notificon_data.cbSize = sizeof(NOTIFYICONDATAA);
    notificon_data.hWnd = FindWindowA(NULL, "NexusShell");
    notificon_data.uID = 72;
    notificon_data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
    notificon_data.uCallbackMessage = WM_USER + 1;
    notificon_data.hIcon = LoadIconA(NULL, "icon.ico");
    strcpy(notificon_data.szTip, "NexusShell");
    Shell_NotifyIconA(NIM_ADD, &notificon_data);
}

LRESULT CALLBACK SystrayCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_SYSTRAY_CREATED) SystrayCreate();
    else if (msg == WM_USER + 1) {
        if (lParam == WM_RBUTTONUP) {
            SDL_Event event;
            event.type = SDL_EVENT_USER;
            event.user.code = 0;
            SDL_PushEvent(&event);
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SystrayRegisterClass() {
    WNDCLASSEXA trayclass;
    ZeroMemory(&trayclass, sizeof(WNDCLASSEXA));
    trayclass.cbSize = sizeof(WNDCLASSEXA);
    trayclass.lpfnWndProc = SystrayCallback;
    trayclass.hInstance = GetModuleHandle(NULL);
    trayclass.lpszClassName = "NexusShell";
    RegisterClassExA(&trayclass);
}

int systray_init(uint32_t flags) {
#ifdef _WIN32
    SystrayRegisterClass();
    SDL_Window* tray_menu = SDL_CreateWindow("nxshtray", 32, 48, SDL_WINDOW_BORDERLESS |
                                                                 SDL_WINDOW_HIDDEN     |
                                                                 SDL_WINDOW_OPENGL     |
                                                                 SDL_WINDOW_POPUP_MENU);
    SDL_Renderer* tray_renderer = SDL_CreateRenderer(tray_menu, NULL, SDL_RENDERER_PRESENTVSYNC);
    // TODO: NULL checks
    WM_SYSTRAY_CREATED = RegisterWindowMessageA("NxSh_SysTrayCreated");
    SystrayCreate();



    return 0;
    Shell_NotifyIconA(NIM_DELETE, &notificon_data);
#else

#endif
    return 0;
}

////// End of tray.cpp /////////

void clear_dialog() {
    ImGui::OpenPopup("Are you sure?");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Are you sure?", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("Are you sure you want to\nclear this item?");
        ImGui::Text("There is no going back!");
        if (ImGui::Button("Yes")) {
            clear_button(CURRENT_SHELL.current_profile, 0, button_properties_to_draw);
            reconfigure_shell(CURRENT_SHELL);
            clear_dialog_shown = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("No")) clear_dialog_shown = false;
        ImGui::EndPopup();
    }
}
// TODO: Create separate thread to generate textures
int LoadTextureFromFile(const char* filename, SDL_Texture** texture_ptr, int width, int height) {
    // TODO: implement keep aspect ratio and padding and compression
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data) {
        LOGVAR("Failed to load image", stbi_failure_reason());
        error_dialog(1, string("Failed to load image.\n", stbi_failure_reason()));
        return -1;
    }
    fprintf(stderr, "Amount of bytes required: %d\n", width * height * channels);

    SDL_Surface* surface = SDL_CreateSurfaceFrom((void*)data, width, height, width * 4, SDL_PIXELFORMAT_RGBA32);

    if (!surface) {
        LOGVAR("Failed to create SDL surface", SDL_GetError());
        error_dialog(0, string(filename, "\nFailed to create SDL surface.\n"));
        return -1;
    }

    *texture_ptr = SDL_CreateTextureFromSurface(renderer, surface);

    if (!texture_ptr) {
        LOGVAR("Failed to create SDL texture", SDL_GetError());
        error_dialog(0, string(filename, "\nFailed to create SDL texture.\n"));
        return -1;
    }

    SDL_DestroySurface(surface);
    stbi_image_free(data);

    return 0;
}
//  URGENT: FREE ALL TEXTURES BEFORE QUITTING OR CLEARING MAP. DON'T USE exit(-1); IMPLEMENR EXIT FUNCTION
string browse(int type) {
    NFD_Init();
    nfdchar_t* outPath = nullptr;
    nfdresult_t result;
    switch (type) {
        case 0: {
            nfdfilteritem_t filterItem[] = { {"All Files", "*" } };
            result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
            break;
        } case 1: {
            nfdfilteritem_t filterItem[] = { {"Image files", "png,jpg,jpeg,ico,bmp" } };
            result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
            break;
        } case 2: {
            result = NFD_PickFolder(&outPath, NULL);
            break;
        }
    }
    NFD_Quit();
    if (result == NFD_OKAY) return outPath;
    return "";
}

void draw_properties(int index) {
    ImGui::OpenPopup("Button Properties");
    static const char* types[] = {"File", "URL", "Command", "Directory"};
    static const char* display_types[] = {"Text", "Image"};
    static int original_type;
    static int original_display_type;
    static string original_action;
    if (set_to_null || button_cleared) {
        original_type         = static_cast<int>(CURRENT_BUTTON.type);
        original_display_type = static_cast<int>(CURRENT_BUTTON.display_type);
        original_action       = CURRENT_BUTTON.action;
        button_cleared        = false;
        set_to_null           = false;
    }
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Button Properties", NULL, ImGuiWindowFlags_NoResize | 
                                                          ImGuiWindowFlags_NoMove)) {
        if (clear_dialog_shown) clear_dialog();
        ImGui::SetWindowSize(ImVec2(450, 250));
        ImGui::Text("Button ID: "); ImGui::SameLine(); ImGui::Text("%s", to_string(index).c_str());
        ImGui::SeparatorText("Display");
        ImGui::Text("Content:"); ImGui::SameLine();
        if (ImGui::BeginCombo("##Display Type", display_types[original_display_type])) {
            for (int i = 0; i < IM_ARRAYSIZE(display_types); ++i) {
                const bool is_selected = (original_display_type == i);
                if (ImGui::Selectable(display_types[i], is_selected)) original_display_type = i;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        switch (original_display_type) {
            case static_cast<int>(Button::display_types::Text): {
                ImGui::Text("Label:"); ImGui::SameLine();
                ImGui::InputText("##Label", &CURRENT_BUTTON.label);
                break;
            } case static_cast<int>(Button::display_types::Image): {
                ImGui::Text("Path:"); ImGui::SameLine();
                ImGui::InputText("##Path", &CURRENT_BUTTON.label); ImGui::SameLine();
                if (ImGui::Button("Browse")) CURRENT_BUTTON.label = browse(1);
                break;
            }
        }
        ImGui::SeparatorText("Behavior");
        ImGui::Text("Type:"); ImGui::SameLine();
        if (ImGui::BeginCombo("##Type", types[original_type])) {
            for (int i = 0; i < IM_ARRAYSIZE(types); ++i) {
                const bool is_selected = (original_type == i);
                if (ImGui::Selectable(types[i], is_selected)) original_type = i;
                if (is_selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        switch (original_type) {
            case static_cast<int>(Button::types::File): {
                ImGui::Text("File:"); ImGui::SameLine();
                ImGui::InputText("##Action", &original_action);
                ImGui::SameLine();
                if (ImGui::Button("Browse")) original_action = browse(0);
                break;
            } case static_cast<int>(Button::types::URL): {
                ImGui::Text("URL:"); ImGui::SameLine();
                ImGui::InputText("##Action", &original_action);
                ImGui::SameLine(); ImGui::TextDisabled("(Not working?)");
                if (ImGui::BeginItemTooltip()) {
                    ImGui::TextUnformatted("URL not working?\n\nYou must prepend https://, http://, git://, \nor any other protocol scheme to the URL.");
                    ImGui::EndTooltip();
                }
                break;
            } case static_cast<int>(Button::types::Command): {
                ImGui::Text("Command:"); ImGui::SameLine();
                ImGui::InputText("##Action", &original_action);
                break;
            } case static_cast<int>(Button::types::Directory): {
                ImGui::Text("Directory:"); ImGui::SameLine();
                ImGui::InputText("##Action", &original_action);
                ImGui::SameLine();
                if (ImGui::Button("Browse")) original_action = browse(2);
                break;
            }
        }
        ImGui::BeginDisabled(original_action.empty() || 
                             (CURRENT_BUTTON.display_type == Button::display_types::Image && 
                              CURRENT_BUTTON.label.empty()));
        if (ImGui::Button("OK")) {
            CURRENT_SHELL.lock.lock();
            if (CURRENT_BUTTON.label_backup != CURRENT_BUTTON.label) {
                CURRENT_BUTTON.default_label = false;
                CURRENT_BUTTON.label_backup = CURRENT_BUTTON.label;
            }
            CURRENT_BUTTON.action = original_action;
            CURRENT_BUTTON.type = static_cast<Button::types>(original_type);
            CURRENT_BUTTON.display_type = static_cast<Button::display_types>(original_display_type);
            if (static_cast<int>(CURRENT_BUTTON.display_type == Button::display_types::Image)) {
                if (CURRENT_BUTTON.thumbnail) SDL_DestroyTexture(CURRENT_BUTTON.thumbnail);
                if (LoadTextureFromFile(CURRENT_BUTTON.label.c_str(), &CURRENT_BUTTON.thumbnail, CURRENT_BUTTON.thumbnail_width, CURRENT_BUTTON.thumbnail_height)) {
                    CURRENT_SHELL.lock.unlock();
                    goto failed; // Error is already displayed from callee. This keeps the popup open and displays the rest of the window.
                }
            }
            original_type = NULL;
            original_action = "";
            set_to_null = true;
            should_draw_button_properties = false;
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["display type"] = to_string(static_cast<int>(CURRENT_BUTTON.display_type));
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["label"] = CURRENT_BUTTON.label;
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["has default label"] = to_string(CURRENT_BUTTON.default_label);
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["action"] = CURRENT_BUTTON.action;
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["type"] = to_string(static_cast<int>(CURRENT_BUTTON.type));
            config_write(CURRENT_SHELL);
            reconfigure_shell(CURRENT_SHELL);
            CURRENT_SHELL.lock.unlock();
        }
failed:
        ImGui::EndDisabled();
        ImGui::SameLine();  // TODO: Move ok/cancel to right
        if (ImGui::Button("Clear")) clear_dialog_shown = true;
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            CURRENT_BUTTON.label = CURRENT_BUTTON.label_backup;
            original_type = NULL;
            set_to_null = true;
            should_draw_button_properties = false;
        }
        ImGui::EndPopup();
    }
}

void draw_button(string label, int index) {
    ImGuiIO& io = ImGui::GetIO();
    float x_size = io.DisplaySize.x - 175;
    if (selected_shell != "") {
        if (ImGui::Button(label.c_str(), ImVec2(x_size / CURRENT_PROFILE.columns,
            io.DisplaySize.y / CURRENT_PROFILE.rows))) {
            button_properties_to_draw = index;
            should_draw_button_properties = true;
            draw_properties(index);
        }
    } else ImGui::Button(std::to_string(index).c_str(), ImVec2(x_size / 6, io.DisplaySize.y / 4));
}

void draw_editor() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    float x_size = io.DisplaySize.x - 175;
    ImGui::SetNextWindowSize(ImVec2(x_size, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(176.0f, 0.0f));
    ImGui::Begin("editor", NULL, im_window_flags);
    if (selected_shell != "") {
        CURRENT_SHELL.lock.lock();
        for (int i = 0; i < CURRENT_PROFILE.columns * CURRENT_PROFILE.rows; ++i) {
            if (!should_draw_shell_properties) {
                if (!static_cast<int>(CURRENT_BUTTON_LOOP.display_type)) {
                    if (CURRENT_BUTTON_LOOP.default_label) draw_button(to_string(i+1), i);
                    else if (CURRENT_BUTTON_LOOP.label.empty()) draw_button("##", i);
                    else draw_button(CURRENT_BUTTON_LOOP.label, i);
                } else {
                    if (CURRENT_BUTTON_LOOP.generate_thumbnail) {
                        //auto generator = [&, CURRENT_BUTTON_M1_LOOP] {
                        //    LoadTextureFromFile(CURRENT_BUTTON_M1_LOOP.label.c_str(), &CURRENT_BUTTON_M1_LOOP.thumbnail, CURRENT_BUTTON_M1_LOOP.thumbnail_width, CURRENT_BUTTON_M1_LOOP.thumbnail_height);
                        //    CURRENT_BUTTON_M1_LOOP.generate_thumbnail = false;
                        //};
                    }
                    if (ImGui::ImageButton((ImTextureID)(intptr_t)CURRENT_BUTTON_LOOP.thumbnail, 
                                            ImVec2(x_size / CURRENT_PROFILE.columns,
                                                    io.DisplaySize.y / CURRENT_PROFILE.rows))) {
                        button_properties_to_draw = i;
                        should_draw_button_properties = true;
                        draw_properties(i);
                    }
                }
            } else draw_button(to_string(i+1), i);
            if (i % CURRENT_PROFILE.columns) ImGui::SameLine();
        }
        CURRENT_SHELL.lock.unlock();
    } else {
        ImGui::BeginDisabled();
        for (int i = 1; i <= 24; ++i) {
            draw_button(std::to_string(i), i);
            if (i % 6) ImGui::SameLine();
        }
        ImGui::EndDisabled();
    }
    ImGui::PopStyleVar(2);
    if (should_draw_button_properties && shells.count(selected_shell)) {
        if (CURRENT_SHELL.ID == selected_shell) draw_properties(button_properties_to_draw);
        else {
            should_draw_button_properties = false;
            selected_shell = "";
        }
    }
    ImGui::End();
}

void draw_id_properties() {
    static int columns, rows;
    static string nickname;
    if (set_to_null) {
        columns = CURRENT_PROFILE.columns;
        rows = CURRENT_PROFILE.rows;
        nickname = CURRENT_SHELL.nickname;
        set_to_null = false;
    }
    ImGui::OpenPopup("Device Properties");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(ImVec2(450, 250));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Device Properties", NULL, ImGuiWindowFlags_NoMove |
                                                          ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Properties for "); ImGui::SameLine();
        ImGui::Text("%s", selected_shell.c_str());
        ImGui::SeparatorText("General");
        ImGui::Text("Nickname:"); ImGui::SameLine();
        ImGui::InputText("##nickname", &CURRENT_SHELL.nickname);
        ImGui::SeparatorText("Layout");
        ImGui::Text("Columns:"); ImGui::SameLine(); 
        ImGui::InputInt("##Columns", &CURRENT_PROFILE.columns);
        ImGui::Text("Rows:"); ImGui::SameLine(); 
        ImGui::InputInt("##Rows", &CURRENT_PROFILE.rows);
        if (CURRENT_PROFILE.columns * CURRENT_PROFILE.rows > 400) {
            CURRENT_PROFILE.columns = columns;
            CURRENT_PROFILE.rows = rows;
        }
        if (ImGui::Button("Ok")) {
            CURRENT_SHELL.lock.lock();
            CURRENT_SHELL.config[selected_shell]["nickname"] = CURRENT_SHELL.nickname;
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["columns"] = to_string(CURRENT_PROFILE.columns);
            CURRENT_SHELL.config[selected_shell]["profiles"][to_string(CURRENT_SHELL.current_profile)]["rows"] = to_string(CURRENT_PROFILE.rows);
            config_write(CURRENT_SHELL);
            int new_grid = CURRENT_PROFILE.columns * CURRENT_PROFILE.rows;
            int old_grid = columns * rows;
            if (new_grid > old_grid) {
                int iter_len = new_grid - CURRENT_PROFILE.buttons.size();
                for (int i = 0; i < iter_len; ++i)
                    CURRENT_PROFILE.buttons.push_back(Button());
            } else if (new_grid < old_grid) {
                vector<Button> swapper;
                int button_count = CURRENT_PROFILE.buttons.size();
                for (int i = 0; i < button_count; ++i) {
                    if (i < new_grid) swapper.push_back(CURRENT_PROFILE.buttons[i]);
                    else clear_button(CURRENT_SHELL.current_profile, 0, i);
                }
                CURRENT_PROFILE.buttons.clear();
                CURRENT_PROFILE.buttons = swapper;
            }
            reconfigure_shell(CURRENT_SHELL);
            CURRENT_SHELL.lock.unlock();
            set_to_null = true;
            should_draw_shell_properties = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            CURRENT_PROFILE.columns = columns;
            CURRENT_PROFILE.rows    = rows;
            CURRENT_SHELL.nickname  = nickname;
            set_to_null = true;
            should_draw_shell_properties = false;
        }
        ImGui::EndPopup();
    }
}

void draw_settings() {
    static int temp_port;
    if (set_to_null) {
        temp_port = port;
        set_to_null = false;
    }
    ImGui::OpenPopup("Settings");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowSize(ImVec2(450, 250));
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Settings", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Port: "); ImGui::SameLine();
        ImGui::InputInt("##Port", &port);
        if (ImGui::Button("Ok")) {
            string portstore = getenv("USERPROFILE");           // TODO: move to rw_portstore
            portstore += "\\AppData\\Roaming\\NexusShell\\";
            if (!exists(portstore)) create_directory(portstore);
            portstore += "portstore";
            ofstream writer(portstore);
            writer << port;
            writer.close();
            set_to_null = true;
            should_draw_settings = false;
            restart_socket = true;
            nx_sock_close(listen_socket);
            string message = "killreason: port changed to " + to_string(port);
            for (auto& shell : shells) {
                send(shell.second.client_socket, message.c_str(), message.length() + 1, 0);
                nx_sock_close(shell.second.client_socket);
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            port = temp_port;
            set_to_null = true;
            should_draw_settings = false;
        }
        ImGui::EndPopup();
    }
}

void draw_sock_reboot() {
    ImGui::OpenPopup("Rebooting Socket");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Rebooting Socket", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
        ImGui::Text("Rebooting socket.\nPlease wait...");
        ImGui::EndPopup();
    }
}

void draw_main() {
    ImGuiIO& io = ImGui::GetIO();
    if (restart_socket) draw_sock_reboot();
    if (should_draw_settings) draw_settings(),
    ImGui::SetNextWindowSize(ImVec2(175, io.DisplaySize.y - 33));
    ImGui::SetNextWindowSizeConstraints(ImVec2(175, 367), ImVec2(175, FLT_MAX));
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::Begin("devices", NULL, im_window_flags);
    ImGui::Text("Connected Devices:\n");
    shells_lock.lock();
    if (!shells.empty()) {
        if (should_draw_shell_properties) draw_id_properties();
        for (unordered_map<string, Shell>::iterator it = shells.begin(); it != shells.end(); ++it) {
            if (it->second.nickname.empty()) {
                if (ImGui::Selectable(it->first.c_str(), selected_shell == it->first))
                    selected_shell = it->first;
            } else {
                if (ImGui::Selectable(it->second.nickname.c_str(), selected_shell == it->first))
                    selected_shell = it->first;
            }
            if (ImGui::BeginPopupContextItem()) {
                selected_shell = it->first;
                if (ImGui::MenuItem("Properties")) should_draw_shell_properties = true;
                if (ImGui::MenuItem("Disconnect")) {
                    send(CURRENT_SHELL.client_socket, "killreason: disconnected by user", strlen("killreason: disconnected by user")+1, 0);
                    nx_sock_close(CURRENT_SHELL.client_socket);
                    selected_shell = "";
                }
                ImGui::EndPopup();
            }
        }
    } else { 
        selected_shell = "";
        ImGui::Text("None"); 
    }
    ImGui::End();
    ImGui::SetNextWindowSize(ImVec2(175, 32));
    ImGui::SetNextWindowSizeConstraints(ImVec2(175, 33), ImVec2(175, 33));
    ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - 32));
    ImGui::Begin("About", NULL, im_window_flags);
    if (ImGui::Button("Settings")) should_draw_settings = true;
    ImGui::End();
    draw_editor();
    shells_lock.unlock();
}

void error_dialog(int type, string message) {
    if (!type) message += SDL_GetError();
    const SDL_MessageBoxButtonData button_data[] = {
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"}
    };
    int buttonid;
    const SDL_MessageBoxData msgbox_data = {
        SDL_MESSAGEBOX_ERROR, NULL, "Error!", message.c_str(),
        SDL_arraysize(button_data), button_data, NULL
    };
    SDL_ShowMessageBox(&msgbox_data, &buttonid);
}

#ifdef _DEBUG 
bool show_demo_window = false;
void draw_performance() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("Performance Statistics", NULL, ImGuiWindowFlags_NoCollapse | 
                                                 ImGuiWindowFlags_NoDocking);
    ImGui::Checkbox("Demo  ", &show_demo_window);
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
    ImGui::SameLine();
    ImGui::Text("Average %.3f ms/f (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::SameLine();
    ImGui::Text("Frames since last input %d", ImGui::GetIO().FrameCountSinceLastInput);
    ImGui::End();
}
#endif
