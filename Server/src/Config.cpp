#include "../include/Header.h"
#include "../include/Config.h"

bool button_cleared = false;

vector<id> ids = {};

void read_config(json& config, id& ID) {
	if (config[config.begin().key()].contains("profiles")) {
		ID.profiles.clear();
		int profile_count = 0;
		for (auto& profile : config[config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
		if (profile_count == 0) {
			profile profile1;
			for (int a = 0; a < profile1.columns * profile1.rows; a++) {
				button button1;
				profile1.buttons.push_back(button1);
			}
			ID.profiles.push_back(profile1);
			return;
		}
		for (int i = 0; i < profile_count; i++) {
			profile profile1;
			json profile_store = config[config.begin().key()]["profiles"][to_string(i)];
			if (profile_store.contains("columns")) profile1.columns = std::stoi(profile_store["columns"].get<string>());
			if (profile_store.contains("rows"))    profile1.rows = std::stoi(profile_store["rows"].get<string>());
			if (profile_store.contains("pages")) {
				int page_count = 0;
				for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
				if (page_count == 0) {
					for (int a = 0; i < profile1.columns * profile1.rows; i++) {
						button button1;
						profile1.buttons.push_back(button1);
					}
					continue;
				}
				for (int j = 0; j < page_count; j++) {
					json page_store = profile_store["pages"][to_string(j)];
					if (page_store.contains("buttons")) {
						for (int k = 0; k < profile1.columns * profile1.rows; k++) {
							button button1;
							if (page_store["buttons"].contains(to_string(k))) {
								json button_store = page_store["buttons"][to_string(k)];
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
									else button1.type = button::types::File;
								} else button1.type = button::types::File;
								if (button_store.contains("action")) button1.action = button_store["action"];
							}
							profile1.buttons.push_back(button1);
						}
					}
				}
			}
			ID.profiles.push_back(profile1);
		}
	} else {
		profile profile1;
		for (int i = 1; i <= profile1.columns * profile1.rows; i++) {
			button button1;
			profile1.buttons.push_back(button1);
		}
		ID.profiles.push_back(profile1);
	}
}

void configure_id(id& ID) {
	ID.config_file = getenv("USERPROFILE");
	ID.config_file += "\\AppData\\Roaming\\NexusShell\\";
	if (!exists(ID.config_file)) create_directory(ID.config_file);
	ID.config_file += ID.ID;
	ID.config_file += ".json";
	cerr << ID.config_file << "\n";
	if (exists(ID.config_file)) {
		ifstream reader(ID.config_file);
		if (reader.peek() != ifstream::traits_type::eof()) {
			try {
				json config = json::parse(reader);
				reader.close();
				read_config(config, ID);
				reconfigure(ID);
				cerr << "configured\n";
				return;
			} catch (...) {
				cerr << "invalid config!\nclearing...\n";		// TODO: Don't clear invalid configs
				//string to_write = "{\"" + ID.ID + "\": {}}";
				//json empty_config = json::parse(to_write);
				//ofstream writer(ID.config_file);
				//writer << empty_config.dump(4);
				//writer.close();
				//reconfigure(ID);
			}
		}
		reader.close();
	}
	profile profile1;
	for (int i = 1; i <= profile1.columns * profile1.rows; i++) {
		button button1;
		profile1.buttons.push_back(button1);
	}
	ID.profiles.push_back(profile1);
	string to_write = "{\"" + ID.ID + "\": {}}";
	json empty_config = json::parse(to_write);
	ofstream writer(ID.config_file);
	writer << empty_config.dump(4);
	writer.close();
	reconfigure(ID);
}

void reconfigure(id& ID) {
	if (exists(ID.config_file)) {
		ifstream reader(ID.config_file);
		json config = json::parse(reader);
		reader.close();
		string configuration = "cfg" + config.dump();
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
	nxsh_config += CURRENT_ID.ID;
	nxsh_config += ".json";
	json to_remove;
	ifstream reader(nxsh_config);
	if (reader && reader.peek() != ifstream::traits_type::eof()) to_remove = json::parse(reader);
	reader.close();
	to_remove[CURRENT_ID.ID]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].erase(to_string(button));
	ofstream writer(nxsh_config);
	writer << to_remove.dump(4);
	writer.close();
	read_config(to_remove, CURRENT_ID);
	button_cleared = true;
	reconfigure(CURRENT_ID);
}

void write_config(vector<string> args, size_t arg_size) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
	nxsh_config += "\\";
	nxsh_config += args[0];
	nxsh_config += ".json";
	json to_write;
	if (exists(nxsh_config)) {
		ifstream reader(nxsh_config);
		if (reader.peek() != ifstream::traits_type::eof()) to_write = json::parse(reader);
		reader.close();
	}

	json* current = &to_write;
	for (size_t i = 0; i < arg_size - 1; ++i) {
		if (current->find(args[i]) == current->end()) (*current)[args[i]] = json::object();
		current = &(*current)[args[i]];
	}
	*current = args.back();

	ofstream writer(nxsh_config);
	writer << to_write.dump(4);
	writer.close();
}
