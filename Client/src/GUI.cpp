#include "../include/GUI.h"
#include "../include/Header.h"

bool done = false;
bool failed_backup;
bool run_setup;
bool hide_failed;

int collumns;
int rows;

ImGuiWindowFlags im_window_flags = 0;

int gui_init() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");  // Enable mative IME (Input Method Engine)
    SDL_Window* window = SDL_CreateWindow("ShortPad", 1280, 720, SDL_WINDOW_OPENGL     |
                                                                 SDL_WINDOW_FULLSCREEN |
                                                                 SDL_WINDOW_HIDDEN    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC |
                                                              SDL_RENDERER_ACCELERATED );
    SDL_ShowWindow(window);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io     = ImGui::GetIO();
    io.IniFilename  = nullptr;   // Disable imgui.ini
    io.LogFilename  = nullptr;   //
    io.ConfigFlags |= ImGuiConfigFlags_EnablePowerSavingMode;

    ImGui::StyleColorsDark();
    ImVec4*     colors  = set_colors();
    ImGuiStyle& style   = set_style();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    screens current_display = get_default_screen();
    hide_failed = true;
    run_setup = true;
    collumns = 6; // TODO: Get default layout
    rows     = 4; //
    do {
        //ImGui_ImplSDL3_WaitForEvent();
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

        if (run_setup && !have_ip)                       draw_setup();
        if (!connected && have_ip || disconnected_modal) draw_disconnected_alert();

        im_window_flags |= ImGuiWindowFlags_NoDecoration
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

ImVec4* set_colors(/*TODO: Add color parameters and default values*/) {
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
ImGuiStyle& set_style(/*TODO: Add style parameters and default valurs*/) {
    ImGuiStyle& style = ImGui::GetStyle();
    style.ItemSpacing      = ImVec2(0.0f, 0.0f);
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize  = 0.0f;
    style.PopupBorderSize  = 0.0f;
    style.FrameBorderSize  = 0.0f;

    return style;
}

void draw_main(screens current) { // TODO: Create window in main to display tabs, close button etc.
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("main", NULL, im_window_flags | ImGuiWindowFlags_NoBringToFrontOnFocus); // Allow performance info to always be on top
    ImGui::SetWindowSize(io.DisplaySize);
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    switch(current) {
        case screens::Home:
            draw_home();
    }
    ImGui::End();
}

void draw_home() {
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 1; i <= (collumns * rows); i++) {
        //TODO: Later do count of buttons in row, and collums drawn for custom layouts (TBIMPL)
        //NOTE: ImGui::GetForegroundDrawlist()->AddRect(...); // nstead of screen size
        if (ImGui::Button(to_string(i).c_str(), ImVec2((io.DisplaySize.x / collumns), 
                                                       (io.DisplaySize.y / rows)))) {
            string message = SHORTCUT_PREFIX + to_string(i);
            sendRes = send(sock, message.c_str(), sizeof(message.c_str()) + 1, 0);
        }
        if (!(i % collumns == 0)) ImGui::SameLine();
    }
}

screens get_default_screen() {
    // TODO: read json
    return screens::Home;
}

void draw_setup() {
    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ItemSpacing = ImVec2(1.0f, 1.0f);
    ImGui::OpenPopup("Setup", im_window_flags);
    if (ImGui::BeginPopupModal("Setup", NULL, im_window_flags)) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetWindowFontScale(2.0f);
        ImGui::BeginDisabled(failed == failed_backup);
        ImGui::Text("Welcome to ShortPad!");
        ImGui::Separator();
        if (!hide_failed) {
            if (failed)    ImGui::Text("Connection failed.");
            if (connected) run_setup = false;
        }
        ImGui::Text("Server IP address:");
        ImGui::InputText("##", ip_address, IM_ARRAYSIZE(ip_address));
        if (ImGui::Button("Connect")) {
            failed        = false;
            failed_backup = false;
            hide_failed   = false;
        }
        ImGui::EndDisabled();
        // TODO: virtual_keyboard();
        ImGui::EndPopup();
    }
}

void draw_disconnected_alert() {
    ImGuiIO&    io    = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.ItemSpacing = ImVec2(1.0f, 1.0f);
    ImGui::OpenPopup("Disconnected", im_window_flags);
    if (ImGui::BeginPopupModal("Disconnected", NULL, im_window_flags)) {
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f,0.5f));
        ImGui::SetWindowFontScale(2.0f);
        failed = false;
        ImGui::Text("Disconnected.");
        ImGui::Text("Attempting to reconnect to %s", ip_address);
        if (ImGui::Button("Change IP address")) {
            disconnected_modal  = false;
            have_ip             = false;
            failed              = true;
            hide_failed         = true;
            run_setup           = true;
            remove_ipstore();
        } 
        ImGui::EndPopup();
    }
}

#ifdef _DEBUG 
bool show_demo_window = false;
void draw_performance() {
    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("Performance Statistics", NULL, ImGuiWindowFlags_NoCollapse);
    ImGui::Checkbox("Demo  ", &show_demo_window);
    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
    ImGui::SameLine();
    ImGui::Text("Average %.3f ms/f (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::SameLine();
    ImGui::Text("Frames since last input %d", ImGui::GetIO().FrameCountSinceLastInput);
    ImGui::End();
}
#endif