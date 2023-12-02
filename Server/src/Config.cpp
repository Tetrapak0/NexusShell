#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/GUI.h"

bool button_cleared = false;
atomic<bool> ids_locked;

unordered_map<string, id> ids = {};

void read_config(id& ID) {
	if (ID.config[ID.config.begin().key()].contains("nickname")) ID.nickname = ID.config[ID.config.begin().key()]["nickname"].get<string>();
	if (ID.config[ID.config.begin().key()].contains("profiles")) {
		ID.profiles.clear();
		int profile_count = 0;
		for (auto& profile : ID.config[ID.config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
		if (!profile_count) {
			profile profile1;
			for (int a = 0; a < profile1.columns * profile1.rows; a++)
				profile1.buttons.push_back(button());
			ID.profiles.push_back(profile1);
			return;
		}
		for (int i = 0; i < profile_count; ++i) {
			profile profile1;
			json& profile_store = ID.config[ID.config.begin().key()]["profiles"][to_string(i)];
			if (profile_store.contains("columns")) profile1.columns = std::stoi(profile_store["columns"].get<string>());
			if (profile_store.contains("rows"))    profile1.rows = std::stoi(profile_store["rows"].get<string>());
			if (profile_store.contains("pages")) {
				int page_count = 0;
				for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
				if (!page_count) {
					for (int a = 0; i < profile1.columns * profile1.rows; ++i) 
						profile1.buttons.push_back(button());
					continue;
				}
				for (int j = 0; j < page_count; ++j) {
					json& page_store = profile_store["pages"][to_string(j)];
					if (page_store.contains("buttons")) {
						for (int k = 0; k < profile1.columns * profile1.rows; ++k) {
							button button1;
							if (page_store["buttons"].contains(to_string(k))) {
								json& button_store = page_store["buttons"][to_string(k)];
								if (button_store.contains("label")) button1.label = button_store["label"];
								button1.label_backup = button1.label;
								if (button_store.contains("has default label")) {
									if (button_store["has default label"] == "1") button1.default_label = true;
									else if (button_store["has default label"] == "0") button1.default_label = false;
									else if ((button1.label == "")) button1.default_label = true;
									else button1.default_label = false;
								}
								if (button_store.contains("type")) {
									if (button_store["type"] == "0") button1.type = button::types::File;
									else if (button_store["type"] == "1") button1.type = button::types::URL;
									else if (button_store["type"] == "2") button1.type = button::types::Command;
									else if (button_store["type"] == "3") button1.type = button::types::Directory;
								}
								if (button_store.contains("action")) button1.action = button_store["action"];
							}
							profile1.buttons.push_back(button1);
						}
					} else for (int i = 1; i <= profile1.columns * profile1.rows; ++i) profile1.buttons.push_back(button());
				}
			} else for (int i = 1; i <= profile1.columns * profile1.rows; ++i) profile1.buttons.push_back(button());
			ID.profiles.push_back(profile1);
		}
	} else {
		profile profile1;
		for (int i = 1; i <= profile1.columns * profile1.rows; ++i)
			profile1.buttons.push_back(button());
		ID.profiles.push_back(profile1);
	}
}

int configure_id(id& ID) {
	ID.config_file = getenv("USERPROFILE");								// TODO: Switch to the client way of file & dir creation
	ID.config_file += "\\AppData\\Roaming\\NexusShell\\";
	if (!exists(ID.config_file)) create_directory(ID.config_file);
	ID.config_file += ID.ID;
	ID.config_file += ".json";
	LOG(ID.config_file);
	if (exists(ID.config_file)) {
		ifstream reader(ID.config_file);
		if (reader.peek() != ifstream::traits_type::eof()) {
			try {
				json config = json::parse(reader);
				reader.close();
				ID.config = config;
				read_config(ID);
				reconfigure(ID);
				if (!ID.nickname.empty())	LOGVAR("Configured", ID.nickname);
				else						LOGVAR("Configured", ID.ID);
				return 0;
			} catch (...) {
				LOG("invalid config!");
				send(ID.sock.ClientSocket, "killreason: invalid config", strlen("killreason: invalid config") + 1, 0);
				closesocket(ID.sock.ClientSocket);
				return 1;
			}
		}
		reader.close();
	}
	profile profile1;
	for (int i = 1; i <= profile1.columns * profile1.rows; ++i)
		profile1.buttons.push_back(button());
	ID.profiles.push_back(profile1);
	string to_write = "{\"" + ID.ID + "\": {\"profiles\": {\"0\": {\"pages\": {\"0\": {\"buttons\": {}}}}}}}";
	ofstream writer(ID.config_file);
	writer << to_write;
	writer.close();
	ID.config = json::parse(to_write);
	reconfigure(ID);
	return 0;
}

void reconfigure(id& ID) {
	json config = ID.config;
	if (config[config.begin().key()].contains("nickname"))	config[config.begin().key()].erase("nickname");
	for (int i = 0; i < ID.profiles.size(); ++i) {
		json& profile_store = config[config.begin().key()]["profiles"][to_string(i)];
		if (profile_store.contains("pages")) {
			int page_count = 0;
			for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
			if (page_count == 0) continue;
			for (int j = 0; j < page_count; ++j) {
				json& page_store = profile_store["pages"][to_string(j)];
				if (page_store.contains("buttons")) {
					for (int k = 0; k < ID.profiles[i].columns * ID.profiles[i].rows; ++k) {
						if (page_store["buttons"].contains(to_string(k))) {
							json& button_store = page_store["buttons"][to_string(k)];
							if (button_store.contains("type"))  button_store.erase("type");
							if (button_store.contains("action")) {
								if (button_store["action"] == "") {} 
								else button_store["action"] = "1";
							}
						}
					}
				}
			}
		}
		string configuration = "cfg" + ID.config.dump();
		ID.sock.iSendResult = send(ID.sock.ClientSocket, configuration.c_str(), configuration.length(), 0);
	}
}

void clear_button(int profile, int page, int button) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell";
	if (!exists(nxsh_config)) {
		create_directory(nxsh_config);
		return;
	}
	nxsh_config += "\\";
	nxsh_config += selected_id;
	nxsh_config += ".json";
	if (CURRENT_ID.config[selected_id]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].contains(to_string(button))) {
		CURRENT_ID.config[selected_id]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].erase(to_string(button));
		ofstream writer(nxsh_config);
		writer << CURRENT_ID.config.dump(4);
		writer.close();
		read_config(CURRENT_ID);
	}
	button_cleared = true;
	LOG("cleared");
}

void write_config(id& ID) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
	nxsh_config += "\\";
	nxsh_config += selected_id;
	nxsh_config += ".json";
	ofstream writer(nxsh_config);
	writer << ID.config.dump(4);
	writer.close();
}
