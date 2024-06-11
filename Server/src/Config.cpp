#include "../include/Header.hpp"
#include "../include/Config.hpp"
#include "../include/GUI.hpp"

bool button_cleared = false;
mutex shells_lock;

unordered_map<string, Shell> shells = {};

void ConfigRead(Shell& shell) {
    if (shell.config[shell.config.begin().key()].contains("nickname"))
        shell.nickname = shell.config[shell.config.begin().key()]["nickname"].get<string>();
    if (shell.config[shell.config.begin().key()].contains("profiles")) {
        shell.profiles.clear();
        int profile_count = 0;
        for (auto& profile : shell.config[shell.config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
        if (!profile_count) {
            Profile profile;
            for (int a = 0; a < profile.size; ++a)
                profile.buttons.push_back(Button());
            shell.profiles.push_back(profile);
            return;
        }
        for (int i = 0; i < profile_count; ++i) {
            Profile profile;
            json& profiles = shell.config[shell.config.begin().key()]["profiles"][to_string(i)];
            if (profiles.contains("columns")) 
                profile.columns = std::stoi(profiles["columns"].get<string>());
            if (profiles.contains("rows")) 
                profile.rows = std::stoi(profiles["rows"].get<string>());
            profile.size = profile.columns * profile.rows;
            if (profiles.contains("pages")) {
                int page_count = 0;
                for (auto& page : profiles["pages"]) if (page.is_object()) page_count++;
                if (!page_count) {
                    for (int j = 0; j < profile.size; ++j) 
                        profile.buttons.push_back(Button());
                    continue;
                }
                for (int j = 0; j < page_count; ++j) {
                    json& pages = profiles["pages"][to_string(j)];
                    if (pages.contains("buttons")) {
                        for (int k = 0; k < profile.size; ++k) {
                            Button button;
                            if (pages["buttons"].contains(to_string(k))) {
                                json& buttons = pages["buttons"][to_string(k)];
                                if (buttons.contains("label")) button.label = buttons["label"];
                                if (buttons.contains("display type")) {
                                    try {
                                        int type = std::stoi(buttons["display type"].get<string>());
                                        if (type < static_cast<int>(Button::display_types::END) && type > -1) {
                                            button.display_type = static_cast<Button::display_types>(type);
                                            if (type == 1) button.generate_thumbnail = true;
                                        }
                                    } catch (...) {}
                                }
                                button.label_backup = button.label;
                                if (buttons.contains("has default label")) {
                                    if (buttons["has default label"] == "1") button.default_label = true;
                                    else if (buttons["has default label"] == "0") button.default_label = false;
                                    else if ((button.label == "")) button.default_label = true;
                                    else button.default_label = false;
                                }
                                if (buttons.contains("type")) {
                                    try {
                                        int type = std::stoi(buttons["type"].get<string>());
                                        if (type < static_cast<int>(Button::types::END) && type > -1) 
                                            button.type = static_cast<Button::types>(type);
                                    } catch (...) {}
                                }
                                if (buttons.contains("action")) button.action = buttons["action"];
                            }
                            profile.buttons.push_back(button);
                        }
                    } else for (int k = 0; k < profile.size; ++k) profile.buttons.push_back(Button());
                }
            } else for (int j = 0; j < profile.size; ++j) profile.buttons.push_back(Button());
            shell.profiles.push_back(profile);
        }
    } else {
        Profile profile;
        for (int i = 0; i < profile.size; ++i)
            profile.buttons.push_back(Button());
        shell.profiles.push_back(profile);
    }
}

int configure_shell(Shell& shell) {
    // TODO: Switch to the client way of file & dir creation
    shell.config_path = string(getenv("APPDATA")) + "\\NexusShell\\";
    if (!exists(shell.config_path)) create_directories(shell.config_path);
    shell.config_path += shell.ID + ".json";
    LOG(shell.config_path);
    if (exists(shell.config_path)) {
        ifstream reader(shell.config_path);
        if (reader.peek() != ifstream::traits_type::eof()) {
            try {
                json config = json::parse(reader);
                reader.close();
                shell.config = config;
                ConfigRead(shell);
                reconfigure_shell(shell);
                if (!shell.nickname.empty()) LOGVAR("Configured", shell.nickname);
                else LOGVAR("Configured", shell.ID);
                return 0;
            } catch (...) {
                LOG("invalid config!");
                send(shell.client_socket, "killreason: invalid config", strlen("killreason: invalid config") + 1, 0);
                nx_sock_close(shell.client_socket);
                return 1;
            }
        }
        reader.close();
    }
    Profile profile;
    for (int i = 0; i < profile.size; ++i)
        profile.buttons.push_back(Button());
    shell.profiles.push_back(profile);
    string out = "{\"" + shell.ID + "\": {\"profiles\": {\"0\": {\"pages\": {\"0\": {\"buttons\": {}}}}}}}";
    ofstream writer(shell.config_path);
    writer << out;
    writer.close();
    shell.config = json::parse(out);
    reconfigure_shell(shell);
    return 0;
}

void reconfigure_shell(Shell& shell) {
    json config = shell.config;
    // TODO: If type == 1, set label to "<image>" (profile/page/button.format) - the client will know what to do
    if (config[config.begin().key()].contains("nickname")) config[config.begin().key()].erase("nickname");
    for (size_t i = 0; i < shell.profiles.size(); ++i) {
        json profiles = config[config.begin().key()]["profiles"][to_string(i)];
        if (profiles.contains("pages")) {
            int page_count = 0;
            for (auto& page : profiles["pages"]) if (page.is_object()) ++page_count;
            if (!page_count) continue;
            for (int j = 0; j < page_count; ++j) {
                json pages = profiles["pages"][to_string(j)];
                if (pages.contains("buttons")) {
                    for (int k = 0; k < shell.profiles[i].columns * shell.profiles[i].rows; ++k) {
                        if (pages["buttons"].contains(to_string(k))) {
                            json buttons = pages["buttons"][to_string(k)];
                            if (buttons.contains("display type"))
                                if (buttons["display type"] == "1")
                                    if (buttons.contains("label"))
                                        buttons["label"] = "<image>";
                            if (buttons.contains("type")) buttons.erase("type");
                            if (buttons.contains("action"))
                                if (!buttons["action"].empty()) buttons["action"] = "1";
                        }
                    }
                }
            }
        }
        string out_cfg = "cfg" + config.dump();
        shell.send_res = send(shell.client_socket, out_cfg.c_str(), out_cfg.length(), 0);
    }
}

void clear_button(int profile, int page, int button) {
    string nxsh_config = string(getenv("APPDATA")) + "\\NexusShell\\";
    if (!exists(nxsh_config)) create_directories(nxsh_config);
    nxsh_config += "\\" + selected_shell + ".json";
    if (CURRENT_SHELL.config[selected_shell]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].contains(to_string(button))) {
        if (CURRENT_SHELL.profiles[profile].buttons[button].display_type == Button::display_types::Image) 
            SDL_DestroyTexture(CURRENT_SHELL.profiles[profile].buttons[button].thumbnail);
        CURRENT_SHELL.config[selected_shell]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].erase(to_string(button));
        ofstream writer(nxsh_config);
        writer << CURRENT_SHELL.config.dump(4);
        writer.close();
        ConfigRead(CURRENT_SHELL);
    }
    button_cleared = true;
    printf("Cleared %d/%d/%d\n", profile, page, button);
}

void config_write(Shell& shell) {
    string nxsh_config = string(getenv("APPDATA")) + "\\NexusShell\\";
    if (!exists(nxsh_config)) create_directories(nxsh_config);
    nxsh_config += "\\";
    nxsh_config += selected_shell;
    nxsh_config += ".json";
    ofstream writer(nxsh_config);
    writer << shell.config.dump(4);
    writer.close();
}
