#include "../include/GUI.h"
#include "../include/Header.h"

bool done = false;
bool write = true;
bool should_wait = false;
ImGuiWindowFlags im_window_flags = 0;

// NOTE: folder structure, shortcut using folder class containing shortcut vector that uses shortcut/folder class

int gui_init()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");	// Enable native IME (Input Method Engine)
	SDL_Window* window = SDL_CreateWindow("ShortPad", 1280, 720, SDL_WINDOW_OPENGL    |
																 SDL_WINDOW_RESIZABLE |
																 SDL_WINDOW_HIDDEN   );
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC | 
															  SDL_RENDERER_ACCELERATED );
	SDL_ShowWindow(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;   // Disable imgui.ini
	io.LogFilename = nullptr;   //
	io.ConfigFlags = ImGuiConfigFlags_EnablePowerSavingMode;

	ImGui::StyleColorsDark(); 
	ImVec4*		colors	= set_colors();
	ImGuiStyle& style	= set_style();

	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	do {
		if (should_wait) ImGui_ImplSDL3_WaitForEvent();
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

		im_window_flags |= ImGuiWindowFlags_NoDecoration
						| ImGuiWindowFlags_AlwaysAutoResize
						| ImGuiWindowFlags_NoResize
						| ImGuiWindowFlags_NoMove;

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
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void prepare_json()	 {
	//vector<string> params = {id, "shortcuts",  "1", "type", "program" }; // TODO: switch to enums / TODO: for loop this and write
	//TODO decide what shold be written here
	//write_config(params, params.size()); // FIXME: Writes twice??
}

ImVec4* set_colors(/*TODO: Add theme parameters and default valurs*/) {
	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]					= ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]			= ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ChildBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]				= ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
	colors[ImGuiCol_FrameBg]				= ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]			= ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive]			= ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg]				= ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive]			= ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab]			= ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered]	= ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark]				= ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab]				= ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button]					= ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered]			= ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive]			= ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Separator]				= ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive]		= ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip]				= ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered]		= ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab]					= ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered]				= ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive]				= ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused]			= ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive]		= ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]			= ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget]			= ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_ModalWindowDimBg]		= ImVec4(0.00f, 0.00f, 0.00f, 0.35f);

	return colors;
}
// TODO: custom themes and styles
ImGuiStyle& set_style(/*TODO: Add style parameters and default valurs*/) {
	ImGuiStyle& style		= ImGui::GetStyle();
	style.ItemSpacing		= ImVec2(1.00f, 1.00f);
	style.ItemInnerSpacing	= ImVec2(1.00f, 1.00f);
	style.GrabMinSize		= 10;
	style.WindowBorderSize	= 0;
	style.ChildBorderSize	= 0;
	style.PopupBorderSize	= 0;
	style.FrameBorderSize	= 0;
	style.TabBorderSize		= 0;
	style.LogSliderDeadzone = 4;

	return style;
}

void draw_main() {
	/*
	NFD_Init();

    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[2] = {{"Source code", "c,cpp,cc"}, {"Headers", "h,hpp"}};
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
    if (result == NFD_OKAY) {
        puts("Success!");
        puts(outPath);
        NFD_FreePath(outPath);
    } else if (result == NFD_CANCEL) puts("User pressed cancel.");
    else printf("Error: %s\n", NFD_GetError());

    NFD_Quit();
	*/
	
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("main", NULL, im_window_flags);
	ImGui::Columns();
	ImGui::Text("Connected Devices:");
	cerr << "empty: " <<ids.empty() << "\n" << "size: " << ids.size() << "\n";
	if (!ids.empty()) {
		cerr << "1\n";
		for (int i = 0; i <= ids.size(); i++) {
			cerr << "2\n";
			if (ImGui::Button((ids[i].ID).c_str())) {
				cerr << "3\n";
			}
		}
	} else ImGui::Text("None");
	ImGui::NextColumn();
	ImGui::SetWindowSize(io.DisplaySize);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::End();
}

// draw_performance() definition
#ifdef _DEBUG 
bool show_demo_window = false;
void draw_performance() {
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("Performance Statistics", NULL);
	ImGui::Checkbox("Demo  ", &show_demo_window);
	if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
	ImGui::SameLine();
	ImGui::Text("Average %.3f ms/f (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::SameLine();
	ImGui::Text("Frames since last input %d", ImGui::GetIO().FrameCountSinceLastInput);
	ImGui::End();
}
#endif