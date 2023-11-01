#include "../include/Header.h"
#include "../include/Config.h"

void read_config(json& config, id* ID) {
	if (config[config.begin().key()].contains("profiles")) {
		ID->profiles.clear();
		int profile_count = 0;
		for (auto& profile : config[config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
		if (profile_count == 0) {
			profile profile1;
			for (int a = 0; a < profile1.columns * profile1.rows; a++) {
				button button1;
				profile1.buttons.push_back(button1);
			}
		}
		for (int i = 0; i < profile_count; i++) {
			profile profile1;
			json profile_store = config[config.begin().key()]["profiles"][to_string(i)];
			if (profile_store.contains("columns")) profile1.columns = std::stoi(profile_store["columns"].get<string>());
			if (profile_store.contains("rows"))    profile1.rows = std::stoi(profile_store["rows"].get<string>());
			if (profile_store.contains("pages")) {
				int page_count = 0;
				for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
				if (page_count == 0)
					for (int a = 0; i < profile1.columns * profile1.rows; i++) {
						button button1;
						profile1.buttons.push_back(button1);
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
									else button1.type = button::types::File;
								} else button1.type = button::types::File;
								if (button_store.contains("action")) button1.action = button_store["action"];
							}
							profile1.buttons.push_back(button1);
						}
					}
				}
			}
			ID->profiles.push_back(profile1);
		}
	} else {
		profile profile1;
		for (int i = 1; i <= profile1.columns * profile1.rows; i++) {
			button button1;
			profile1.buttons.push_back(button1);
		}
		ID->profiles.push_back(profile1);
	}
}

void configure_id(id* id) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell\\";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
	nxsh_config += id->ID;
	nxsh_config += ".json";
	cerr << nxsh_config << "\n";
	if (exists(nxsh_config)) {
		ifstream reader(nxsh_config);
		if (reader.peek() != ifstream::traits_type::eof()) {
			try {
				json config = json::parse(reader);
				read_config(config, id);
				reader.close();
				cerr << "configured\n";
				send_config = true;
				return;
			} catch (...) {
				cerr << "invalid config!\nclearing...\n";
				string to_write = "{\"" + id->ID + "\": {}}";
				json empty_config = json::parse(to_write);
				ofstream writer(nxsh_config);
				writer << empty_config.dump(4);
				writer.close();
				send_config = true;
			}
		}
		reader.close();
	}
	profile profile1;
	for (int i = 1; i <= profile1.columns * profile1.rows; i++) {
		button button1;
		profile1.buttons.push_back(button1);
	}
	id->profiles.push_back(profile1);
	string to_write = "{\"" + id->ID + "\": {}}";
	json empty_config = json::parse(to_write);
	ofstream writer(nxsh_config);
	writer << empty_config.dump(4);
	writer.close();
	send_config = true;
}

void clear_button(int profile, int page, int button) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell";
	if (!exists(nxsh_config)) {
		create_directory(nxsh_config);
		return;
	}
	nxsh_config += "\\";
	nxsh_config += ids[selected_id].ID;
	nxsh_config += ".json";
	json to_remove;
	ifstream reader(nxsh_config);
	if (reader && reader.peek() != ifstream::traits_type::eof()) to_remove = json::parse(reader);
	reader.close();
	to_remove[ids[selected_id].ID]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].erase(to_string(button));
	ofstream writer(nxsh_config);
	writer << to_remove.dump(4);
	writer.close();
	read_config(to_remove, &ids[selected_id]);
	button_cleared = true;
	send_config = true;
}

void write_config(vector<string> args, int arg_size) {
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
