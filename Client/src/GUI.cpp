#include "../include/GUI.h"
#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/Client.h"

int current_profile = 0; // TODO:

bool done = false;
bool failed_backup;
bool run_setup;
bool hide_failed;
bool disconnected_modal = false;

vector<profile> profiles = {};

ImGuiStyle default_style;
ImGuiWindowFlags im_window_flags = 0;

int gui_init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0");
    SDL_Window* window = SDL_CreateWindow("NexusShell", 1280, 720, SDL_WINDOW_OPENGL     |
                                                                 SDL_WINDOW_FULLSCREEN |
                                                                 SDL_WINDOW_HIDDEN    );
    if (window == nullptr) error_dialog(0, "Failed to initialize window.\n");
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC |
                                                              SDL_RENDERER_ACCELERATED );
    if (renderer == nullptr) error_dialog(0, "Failed to initialize renderer.\n");
    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io     = ImGui::GetIO();
    io.IniFilename  = nullptr;   // Disable imgui.ini
    io.LogFilename  = nullptr;   //
    io.ConfigFlags |= ImGuiConfigFlags_EnablePowerSavingMode;

    ImGui::StyleColorsDark();
    ImVec4*     colors  = set_colors();
    default_style       = ImGui::GetStyle();
    ImGuiStyle& style   = set_style();

	static const ImWchar glyph_range[] = {0x0020, 0xFFFF};
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(Helvetica_compressed_data_base85, 16, NULL, glyph_range);

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    screens current_display = get_default_screen();
    
    hide_failed = true;
    run_setup   = true;

    do {
        ImGui_ImplSDL3_WaitForEvent();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && 
                                                 event.window.windowID == SDL_GetWindowID(window)))
                done = true;
        }
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (run_setup && (!have_ip || !have_port))  draw_setup();
        if ((!connected && have_ip && have_port) && !kill || disconnected_modal) draw_disconnected_alert();
        if (kill && !connected) draw_killed();

        im_window_flags |= ImGuiWindowFlags_NoDecoration
                        |  ImGuiWindowFlags_NoDocking
                        |  ImGuiWindowFlags_NoScrollbar
                        |  ImGuiWindowFlags_AlwaysAutoResize
                        |  ImGuiWindowFlags_NoResize
                        |  ImGuiWindowFlags_NoMove;

        draw_main(current_display);
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
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

ImVec4* set_colors() {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
    colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);

    return colors;
}
// TODO: custom themes and styles
ImGuiStyle& set_style() {
    ImGuiStyle& style = ImGui::GetStyle();
    style.ItemSpacing       = ImVec2(0.0f, 0.0f);
    style.ItemInnerSpacing  = ImVec2(0.0f, 0.0f);
    style.FramePadding      = ImVec2(0.0f, 0.0f);
    style.WindowPadding     = ImVec2(0.0f, 0.0f);
    style.WindowBorderSize  = 0.0f;
    style.ChildBorderSize   = 0.0f;
    style.PopupBorderSize   = 0.0f;
    style.FrameBorderSize   = 0.0f;

    return style;
}

void draw_main(screens current) { // TODO: Create window in main to display tabs, close button etc.
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::Begin("main", NULL, im_window_flags | ImGuiWindowFlags_NoBringToFrontOnFocus); // Allow performance info to always be on top
    switch(current) {
        case screens::Home:
            draw_home();
    }
    ImGui::End();
}

void draw_button(string label, int index) {
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::Button(label.c_str(), ImVec2((io.DisplaySize.x / CURRENT_PROFILE.columns), 
                                            (io.DisplaySize.y / CURRENT_PROFILE.rows)))) {
        string message = SHORTCUT_PREFIX + to_string(index);
        send_result = send(sock, message.c_str(), message.length() + 1, 0);
    }
}

void draw_home() {
    ImGuiIO& io = ImGui::GetIO();
    if (!reconfiguring.load() && !profiles.empty() && !CURRENT_PROFILE.buttons.empty()) {
        for (int i = 1; i <= (CURRENT_PROFILE.columns * CURRENT_PROFILE.rows); ++i) {
            ImGui::BeginDisabled(CURRENT_BUTTOM_M1_LOOP.action.empty());
            if (CURRENT_BUTTOM_M1_LOOP.default_label) draw_button(to_string(i), i);
            else if (CURRENT_BUTTOM_M1_LOOP.label.empty()) draw_button("##", i);
            else draw_button(CURRENT_BUTTOM_M1_LOOP.label, i);
            if (i % CURRENT_PROFILE.columns != 0) ImGui::SameLine();
            ImGui::EndDisabled();
        }
    } else {
        ImGui::BeginDisabled();
        for (int i = 1; i <= 24; ++i) {
            ImGui::Button(to_string(i).c_str(), ImVec2((io.DisplaySize.x / 6), (io.DisplaySize.y / 4)));
            if (i % 6 != 0) ImGui::SameLine();
        }
        ImGui::EndDisabled();
    }
}

screens get_default_screen() {
    // TODO: read file
    return screens::Home;
}

void set_popup_style() {
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, default_style.ItemSpacing);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, default_style.ItemInnerSpacing);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, default_style.FramePadding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, default_style.WindowPadding);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, default_style.WindowBorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, default_style.ChildBorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, default_style.PopupBorderSize);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, default_style.FrameBorderSize);
}

void draw_killed() {
    set_popup_style();
    ImGui::OpenPopup("Killed");
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Killed", NULL, im_window_flags)) {
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("Disconnected.");
        ImGui::Text(kill_reason.c_str());
        if (ImGui::Button("Reconnect")) {
            kill = false;
            disconnected_modal = true;
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(8);
}

void draw_setup() {
    set_popup_style();
    ImGui::OpenPopup("Setup", im_window_flags);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Setup", NULL, im_window_flags)) {
        ImGui::SetWindowFontScale(2.0f);
        ImGui::BeginDisabled(failed == failed_backup);
        ImGui::Text("Welcome to NexusShell!");
        ImGui::Separator();
        if (!hide_failed) {
            if (failed) ImGui::Text("Connection failed.");
            if (connected) run_setup = false;
        }
        ImGui::Text("Server IP address:");
        ImGui::InputText("##IP", ip_address, 16);
        ImGui::Text("Server port:");
        ImGui::InputInt("##port", &port);
        if (port > 65535) port = 27015;
        if (ImGui::Button("Connect")) {
            failed        = false;
            failed_backup = false;
            hide_failed   = false;
        }
        ImGui::EndDisabled();
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(8);
}
// FIXME: Sluggish font
void draw_disconnected_alert() {
    if (connected) disconnected_modal = false;
    set_popup_style();
    ImGui::OpenPopup("Disconnected", im_window_flags);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Disconnected", NULL, im_window_flags)) {
        ImGui::SetWindowFontScale(2.0f);
        failed = false;
        ImGui::Text("Disconnected.");
        ImGui::Text("Attempting to reconnect to %s:%i", ip_address, port);
        if (ImGui::Button("Change server info")) {
            disconnected_modal  = false;
            have_ip             = false;
            have_port           = false;
            failed              = true;
            hide_failed         = true;
            run_setup           = true;
            remove_ipstore();
            remove_portstore();
            remove_config();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar(8);
}

void error_dialog(int type, string message) {
	if (!type) message += SDL_GetError();
    LOG(message);
	int buttonid;
	SDL_MessageBoxButtonData button_data[] = { 
        {SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"}
    };
	SDL_MessageBoxData msgbox_data = {
		SDL_MESSAGEBOX_ERROR,		NULL,
		"Error!",					message.c_str(),
		SDL_arraysize(button_data), button_data,
        NULL
	};
	SDL_ShowMessageBox(&msgbox_data, &buttonid);
}

#ifdef _DEBUG 
bool show_demo_window = false;
void draw_performance() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("Performance Statistics", NULL, ImGuiWindowFlags_NoCollapse | 
                                                 ImGuiWindowFlags_NoDocking );
    ImGui::Checkbox("Demo  ", &show_demo_window);
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
    ImGui::SameLine();
    ImGui::Text("Average %.3f ms/f (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::SameLine();
    ImGui::Text("Frames since last input %d", ImGui::GetIO().FrameCountSinceLastInput);
    ImGui::End();
}
#endif
