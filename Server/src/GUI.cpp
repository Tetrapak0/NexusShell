#include "../include/GUI.h"
#include "../include/Header.h"
#include "../include/Helvetica.h"
#include "../include/Roboto.h"
#include "../include/JUST_Sans.h"
#include "../include/Verdana.h"

bool done		   		    = false;
bool should_draw_properties = false;

int columns;
int columns_prev;
int rows;
int rows_prev;
int properties_to_draw;
int selected_id = -1;

ImGuiWindowFlags im_window_flags = 0;

// NOTE: folder structure, shortcut using folder class containing shortcut vector that uses shortcut/folder class


int gui_init() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	SDL_Window* window = SDL_CreateWindow("ShortPad", 1280, 720, SDL_WINDOW_OPENGL    |
																 SDL_WINDOW_RESIZABLE |
																 SDL_WINDOW_HIDDEN   );
	if (window == nullptr) SDL_ERROR("window")
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) SDL_ERROR("renderer")
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

	NFD_Init();
	nfdchar_t* outPath;
	nfdfilteritem_t filterItem = {"All Files", "*"};
	//nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);

	rows	 = 4;
	columns	 = 6;

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

		im_window_flags |= ImGuiWindowFlags_NoDecoration
						|  ImGuiWindowFlags_NoBringToFrontOnFocus
						|  ImGuiWindowFlags_AlwaysAutoResize
						|  ImGuiWindowFlags_NoResize
						|  ImGuiWindowFlags_NoMove;

		draw_main();
#ifdef _DEBUG
		draw_performance();
#endif

		ImGui::Render();
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	} while (!done);
	NFD_Quit();
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

void prepare_json()	{
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

int tray_init() {
#ifdef _WIN32
	Tray::Tray tray("ShortPad", "..\\..\\icon.ico");
#else 
	Tray::Tray tray("ShortPad", "../../icon.ico");
#endif
	tray.addEntry(Tray::Button("Show Window\0", [&] { ShowWindow(FindWindow(NULL, L"ShortPad"), SW_NORMAL);
													  SetFocus(FindWindow(NULL, L"ShortPad")); }));
	tray.addEntry(Tray::Separator());
	tray.addEntry(Tray::Button("Exit\0", [&] {done = true; exit(0); }));
	tray.run();
	return 0;
}

void draw_properties(int button) {
	ImGui::OpenPopup("Properties");
	const char* types[] = {"File", "URL", "Command"};
	int selected_type = 0;
	const char* previous_value = types[selected_type];
	if (ImGui::BeginPopupModal("Properties", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
		ImGui::SetWindowSize(ImVec2(300, 150));
		ImGui::Text("Type"); ImGui::SameLine();
		if (ImGui::BeginCombo("##Type", previous_value)) {
			for (int i = 0; i < IM_ARRAYSIZE(types); i++) {
				const bool is_selected = (selected_type == i);
				if (ImGui::Selectable(types[i], is_selected)) selected_type = i;
				if (is_selected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
 		if (ImGui::Button("OK")) should_draw_properties = false;
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) should_draw_properties = false;
		ImGui::EndPopup();
	}
}

void draw_editor() {
	ImGuiIO& io = ImGui::GetIO();
	float x_size = io.DisplaySize.x - 185;
	ImGui::Begin("editor", NULL, im_window_flags);
	ImGui::SetWindowSize(ImVec2(x_size, io.DisplaySize.y));
	ImGui::SetWindowPos(ImVec2(186.0f, 0.0f));
	ImGui::BeginDisabled(selected_id == -1);
	for (int i = 1; i <= (columns * rows); i++) {
		if (ImGui::Button((std::to_string(i)).c_str(), ImVec2(x_size / columns, io.DisplaySize.y / rows))) {
			draw_properties(i);
			should_draw_properties = true;
			properties_to_draw	   = i;
		}
		if (i % columns != 0) ImGui::SameLine();
	}
	ImGui::EndDisabled();
	if (should_draw_properties) draw_properties(properties_to_draw);
	ImGui::End();
}

void draw_main() {
	/*
	nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);
	if (result == NFD_OKAY) {
		NFD_FreePath(outPath);
	} else if (result == NFD_CANCEL) puts("User pressed cancel.");
	else printf("Error: %s\n", NFD_GetError());
	*/
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Begin("devicelist", NULL, im_window_flags);
	ImGui::SetWindowSize(ImVec2(185, io.DisplaySize.y));
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::Text("Connected Devices:\n");
	if (!ids.empty()) {
		for (int i = 0; i <= ids.size() - 1; i++) {
			if (ImGui::Selectable((ids[i].ID).c_str(), selected_id == i)) {
				selected_id = i;
			}
		}
	} else ImGui::Text("None");
	ImGui::End();
	draw_editor();
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