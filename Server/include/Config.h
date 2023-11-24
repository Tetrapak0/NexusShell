#pragma once

#define CURRENT_ID				ids[selected_id]
#define CURRENT_ID_OBJ			id_map[CURRENT_ID]
#define CURRENT_PROFILE			id_map[CURRENT_ID].profiles[id_map[CURRENT_ID].current_profile]
#define CURRENT_PROFILE_PAR		ID.profiles[ID.current_profile]

#define CURRENT_BUTTON			CURRENT_PROFILE.buttons[index] // TODO: Change this to actual page
#define CURRENT_BUTTON_M1_LOOP  CURRENT_PROFILE.buttons[i-1]

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

#include "../include/Server.h"

#include "../../json.hpp"

struct sockinfo;

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::unordered_map;

using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::create_directory;

using json = nlohmann::ordered_json;

class button {
public:
	string label = "";
	string label_backup = label;
	bool   default_label = true;
	enum   types { File, URL, Command, Directory, };
	types  type = File;
	string action;
};

class profile {
public:
	int	columns = 6;
	int	rows = 4;
	int	current_page = 0;
	vector<button> buttons;
};

class id {
public:
	int current_profile = 0;
	bool locked = false;
	string ID;
	string nickname;
	string config_file;
	json config;
	sockinfo sock;
	vector<profile> profiles;
	id() {}
	id(string in_ID) : ID(in_ID) {};
	id(const id& ID) {
		this->ID = ID.ID;
		this->config_file = ID.config_file;
		this->profiles = ID.profiles;
	}
	id(const id* ID) {
		this->ID = ID->ID;
		this->config_file = ID->config_file;
		this->profiles = ID->profiles;
	}
	bool operator==(const id& other) const {
		return this->ID == other.ID;
	}
	id operator=(const id& other) {
		this->config_file = other.config_file;
		this->sock = other.sock;
		this->profiles = other.profiles;
		return this;
	}
};

extern unordered_map<string, id> id_map;

extern vector<string> ids;

extern bool ids_locked;

extern void clear_button(int profile, int page, int button);
extern void configure_id(id& ID);
extern void reconfigure(id& ID);
extern void write_config(vector<string> args, size_t arg_size);
