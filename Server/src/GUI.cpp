#include "../include/Header.h"
#include "../include/GUI.h"
#include "../include/Config.h"

bool done		   				   = false;
bool should_draw_button_properties = false;
bool should_draw_id_properties     = false;
bool should_draw_settings		   = false;
bool set_to_null				   = true;
bool clear_dialog_shown			   = false;

int button_properties_to_draw;

string selected_id = "";

ImGuiWindowFlags im_window_flags = 0;

// TODO: switch actions to wide strings

int gui_init() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
	SDL_Window* window = SDL_CreateWindow("NexusShell", 1280, 720, SDL_WINDOW_OPENGL  |
																 SDL_WINDOW_RESIZABLE |
																 SDL_WINDOW_HIDDEN   );
	if (window == nullptr) { error_dialog(0, "Failed to initialize window.\n"); exit(-1); }
	SDL_SetWindowMinimumSize(window, 854, 480);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) { error_dialog(0, "Failed to initialize renderer.\n"); exit(-1); }
	SDL_ShowWindow(window);
	// TODO: add silent startup -- no show window
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = nullptr;   // Disable imgui.ini
	io.LogFilename = nullptr;   //
	io.ConfigFlags = ImGuiConfigFlags_EnablePowerSavingMode;

	ImGui::StyleColorsDark(); 
	ImVec4*		colors	= set_colors();
	ImGuiStyle& style	= set_style();

	static const ImWchar glyph_range[] = {0x0020, 0xFFFF};
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(Helvetica_compressed_data_base85, 16, NULL, glyph_range);

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
// TODO: custom style
ImGuiStyle& set_style() {
	ImGuiStyle& style		= ImGui::GetStyle();
	style.ItemSpacing		= ImVec2(0.0f, 0.0f);
	style.ItemInnerSpacing	= ImVec2(0.0f, 0.0f);
	style.GrabMinSize		= 10;
	style.WindowBorderSize	= 0;
	style.ChildBorderSize	= 0;
	style.PopupBorderSize	= 0;
	style.FrameBorderSize	= 0;
	style.TabBorderSize		= 0;
	style.LogSliderDeadzone = 4;

	return style;
}

int tray_init() {		// TODO: Switch to another tray system
	Tray::Tray tray(L"NexusShell", "icon.ico");
	tray.addEntry(Tray::Button("Show Window\0", [&] { ShowWindow(FindWindow(NULL, L"NexusShell"), SW_NORMAL);
													  SetFocus(FindWindow(NULL, L"NexusShell")); }));
	tray.addEntry(Tray::Separator());
	tray.addEntry(Tray::Button("Exit\0", [&] {done = true; 	tray.exit();}));
	tray.run();
	exit(0);
	return 0;
}

void clear_dialog() {
	ImGui::OpenPopup("Are you sure?");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Are you sure?", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
		ImGui::Text("Are you sure you want to\nclear this item?");
		ImGui::Text("There is no going back!");
		if (ImGui::Button("Yes")) {
			clear_button(CURRENT_ID.current_profile, 0, button_properties_to_draw);
			reconfigure(CURRENT_ID);
			clear_dialog_shown = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("No")) clear_dialog_shown = false;
		ImGui::EndPopup();
	}
}

string browse(int type) {
	NFD_Init();
	nfdchar_t* outPath = nullptr;
	nfdresult_t result;
	switch (type) {
		case 0: {
			nfdfilteritem_t filterItem[] = { {"All Files", "*" } };
			result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
			break;
		}
		case 1: 
			result = NFD_PickFolder(&outPath, NULL);
			break;
	}
	NFD_Quit();
	if (result == NFD_OKAY) return outPath;
	return "";
}

void draw_properties(int index) {
	ImGui::OpenPopup("Button Properties");
	static const char* types[] = { "File", "URL", "Command", "Directory" };
	static int original_type;
	static string original_action;
	if (set_to_null || button_cleared) {
		original_type	= CURRENT_BUTTON.type;
		original_action = CURRENT_BUTTON.action;
		button_cleared	= false;
		set_to_null		= false;
	}
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Button Properties", NULL, ImGuiWindowFlags_NoResize | 
														  ImGuiWindowFlags_NoMove)) {
		if (clear_dialog_shown) clear_dialog();
		ImGui::SetWindowSize(ImVec2(450, 250));
		ImGui::Text("Button ID: "); ImGui::SameLine(); ImGui::Text(to_string(index).c_str());
		ImGui::Text("Label:"); ImGui::SameLine();
		ImGui::InputText("##Label", &CURRENT_BUTTON.label);
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
			case button::types::File: {
				ImGui::Text("File:"); ImGui::SameLine();
				ImGui::InputText("##Action", &original_action);
				ImGui::SameLine();
				if (ImGui::Button("Browse")) original_action = browse(0);
				break;
			}
			case button::types::URL: {
				ImGui::Text("URL:"); ImGui::SameLine();
				ImGui::InputText("##Action", &original_action);
				ImGui::SameLine(); ImGui::TextDisabled("(Not working?)");
				if (ImGui::BeginItemTooltip()) {
					ImGui::TextUnformatted("URL not working?\n\nYou must prepend https://, http://, git://, \nor any other protocol scheme to the URL.");
					ImGui::EndTooltip();
				}
				break;
			}
			case button::types::Command: {
				ImGui::Text("Command:"); ImGui::SameLine();
				ImGui::InputText("##Action", &original_action);
				break;
			}
			case button::types::Directory: {
				ImGui::Text("Directory:"); ImGui::SameLine();
				ImGui::InputText("##Action", &original_action);
				ImGui::SameLine();
				if (ImGui::Button("Browse")) original_action = browse(1);
				break;
			}
		}
		ImGui::BeginDisabled(original_action == "");
		if (ImGui::Button("OK")) {
			while (CURRENT_ID.locked.load()) {}
			CURRENT_ID.locked.exchange(true);
			if (CURRENT_BUTTON.label_backup != CURRENT_BUTTON.label) {
				CURRENT_BUTTON.default_label = false;
				CURRENT_BUTTON.label_backup = CURRENT_BUTTON.label;
			}
			CURRENT_BUTTON.type = static_cast<button::types>(original_type);
			CURRENT_BUTTON.action = original_action;
			original_type = NULL;
			original_action = "";
			set_to_null = true;
			should_draw_button_properties = false;
			CURRENT_ID.config[selected_id]["profiles"][to_string(CURRENT_ID.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["label"] = CURRENT_BUTTON.label;
			CURRENT_ID.config[selected_id]["profiles"][to_string(CURRENT_ID.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["has default label"] = to_string(CURRENT_BUTTON.default_label);
			CURRENT_ID.config[selected_id]["profiles"][to_string(CURRENT_ID.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["action"] = CURRENT_BUTTON.action;
			CURRENT_ID.config[selected_id]["profiles"][to_string(CURRENT_ID.current_profile)]["pages"][to_string(CURRENT_PROFILE.current_page)]["buttons"][to_string(button_properties_to_draw)]["type"] = to_string(CURRENT_BUTTON.type);
			write_config(CURRENT_ID);
			reconfigure(CURRENT_ID);
			CURRENT_ID.locked.exchange(false);
		}
		ImGui::EndDisabled();
		ImGui::SameLine();
		if (ImGui::Button("Clear")) clear_dialog_shown = true;;
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
	if (selected_id != "") {
		if (ImGui::Button(label.c_str(), ImVec2(x_size / CURRENT_PROFILE.columns,
			io.DisplaySize.y / CURRENT_PROFILE.rows))) {
			draw_properties(index - 1);
			should_draw_button_properties = true;
			button_properties_to_draw = index - 1;
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
	if (selected_id != "") {
		while (CURRENT_ID.locked.load()) {}
		CURRENT_ID.locked.exchange(true);
		for (int i = 1; i <= CURRENT_PROFILE.columns * CURRENT_PROFILE.rows; ++i) {
			if (!should_draw_id_properties) {
				if (CURRENT_BUTTON_M1_LOOP.default_label) draw_button(to_string(i), i);
				else if (CURRENT_BUTTON_M1_LOOP.label.empty()) draw_button("##", i);
				else draw_button(CURRENT_BUTTON_M1_LOOP.label, i);
			} else draw_button(to_string(i), i);
			if (i % CURRENT_PROFILE.columns != 0) ImGui::SameLine();
		}
		CURRENT_ID.locked.exchange(false);
	} else {
		ImGui::BeginDisabled();
		for (int i = 1; i <= 24; ++i) {
			draw_button(std::to_string(i), i);
			if (i % 6 != 0) ImGui::SameLine();
		}
		ImGui::EndDisabled();
	}
	ImGui::PopStyleVar(2);
	if (should_draw_button_properties && ids.count(selected_id)) {
		if (CURRENT_ID.ID == selected_id) draw_properties(button_properties_to_draw);
		else {
			should_draw_button_properties = false;
			selected_id = "";
		}
	}
	ImGui::End();
}

void draw_id_properties() {
	static int columns;
	static int rows;
	static string nickname;
	if (set_to_null) {
		columns = CURRENT_PROFILE.columns;
		rows = CURRENT_PROFILE.rows;
		nickname = CURRENT_ID.nickname;
		set_to_null = false;
	}
	ImGui::OpenPopup("Device Properties");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowSize(ImVec2(450, 250));
	ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Device Properties", NULL, ImGuiWindowFlags_NoMove |
														  ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Properties for "); ImGui::SameLine();
		ImGui::Text(selected_id.c_str());
		ImGui::SeparatorText("General");
		ImGui::Text("Nickname:"); ImGui::SameLine();
		ImGui::InputText("##nickname", &CURRENT_ID.nickname);
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
			while (CURRENT_ID.locked.load()) {}
			CURRENT_ID.locked.exchange(true);
			CURRENT_ID.config[selected_id]["nickname"] = CURRENT_ID.nickname;
			CURRENT_ID.config[selected_id]["profiles"][to_string(CURRENT_ID.current_profile)]["columns"] = to_string(CURRENT_PROFILE.columns);
			CURRENT_ID.config[selected_id]["profiles"][to_string(CURRENT_ID.current_profile)]["rows"] = to_string(CURRENT_PROFILE.rows);
			write_config(CURRENT_ID);
			int new_grid = CURRENT_PROFILE.columns * CURRENT_PROFILE.rows;
			int old_grid = columns * rows;
			if (new_grid > old_grid) {
				int iter_len = new_grid - CURRENT_PROFILE.buttons.size();
				for (int i = 0; i < iter_len; ++i) {
					button button1;
					CURRENT_PROFILE.buttons.push_back(button1);
				}
			} else if (new_grid < old_grid) {
				vector<button> swapper;
				int btn_size = CURRENT_PROFILE.buttons.size();
				for (int i = 0; i < btn_size; ++i) {
					if (i < new_grid) swapper.push_back(CURRENT_PROFILE.buttons[i]);
					else clear_button(CURRENT_ID.current_profile, 0, i);
				}
				CURRENT_PROFILE.buttons.clear();
				CURRENT_PROFILE.buttons = swapper;
			}
			reconfigure(CURRENT_ID);
			CURRENT_ID.locked.exchange(false);
			set_to_null = true;
			should_draw_id_properties = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel")) {
			CURRENT_PROFILE.columns = columns;
			CURRENT_PROFILE.rows = rows;
			CURRENT_ID.nickname = nickname;
			set_to_null = true;
			should_draw_id_properties = false;
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
			string portstore = getenv("USERPROFILE");			// TODO: move to rw_portstore
			portstore += "\\AppData\\Roaming\\NexusShell\\";
			if (!exists(portstore)) create_directory(portstore);
			portstore += "portstore";
			ofstream writer(portstore);
			writer << port;
			writer.close();
			set_to_null = true;
			should_draw_settings = false;
			restart_socket = true;
			closesocket(global_sock.ListenSocket);
			string message = "killreason: port changed to " + to_string(port);
			for (auto& ID : ids) {
				send(ID.second.sock.ClientSocket, message.c_str(), message.length() + 1, 0);
				closesocket(ID.second.sock.ClientSocket);
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
	while (ids_locked.load()) {}
	ids_locked.exchange(true);
	if (!ids.empty()) {
		if (should_draw_id_properties) draw_id_properties();
		for (unordered_map<string, id>::iterator it = ids.begin(); it != ids.end(); ++it) {
			if (it->second.nickname.empty()) {
				if (ImGui::Selectable(it->first.c_str(), selected_id == it->first))
					selected_id = it->first;
			} else {
				if (ImGui::Selectable(it->second.nickname.c_str(), selected_id == it->first))
					selected_id = it->first;
			}
			if (ImGui::BeginPopupContextItem()) {
				selected_id = it->first;
				if (ImGui::MenuItem("Properties")) should_draw_id_properties = true;
				if (ImGui::MenuItem("Disconnect")) {
					send(CURRENT_ID.sock.ClientSocket, "killreason: disconnected by user", strlen("killreason: disconnected by user")+1, 0);
					closesocket(CURRENT_ID.sock.ClientSocket);
					selected_id = "";
				}
				ImGui::EndPopup();
			}
		}
	} else { 
		selected_id = "";
		ImGui::Text("None"); 
	}
	ids_locked.exchange(false);
	ImGui::End();
	ImGui::SetNextWindowSize(ImVec2(175, 32));
	ImGui::SetNextWindowSizeConstraints(ImVec2(175, 33), ImVec2(175, 33));
	ImGui::SetNextWindowPos(ImVec2(0.0f, io.DisplaySize.y - 32));
	ImGui::Begin("About", NULL, im_window_flags);
	if (ImGui::Button("Settings")) should_draw_settings = true;
	ImGui::End();
	draw_editor();
}

void error_dialog(int type, string message) {
	if (!type) message += SDL_GetError();
	const SDL_MessageBoxButtonData button_data[] = {
		{SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 0, "OK"}
	};
	int buttonid;
	const SDL_MessageBoxData msgbox_data = {
		SDL_MESSAGEBOX_ERROR,		NULL,
		"Error!",					message.c_str(),
		SDL_arraysize(button_data), button_data,	NULL
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
