#pragma once

#define CURRENT_ID				ids[selected_id]
#define CURRENT_PROFILE			CURRENT_ID.profiles[CURRENT_ID.current_profile]
#define CURRENT_PROFILE_PAR		ID.profiles[ID.current_profile]

#define CURRENT_BUTTON			CURRENT_PROFILE.buttons[index] // TODO: Change this to actual page
#define CURRENT_BUTTON_LOOP  CURRENT_PROFILE.buttons[i]
#define CURRENT_BUTTON_M1_LOOP  CURRENT_PROFILE.buttons[i-1]

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>

#include "../include/Server.h"

#include "../../external/json.hpp"

struct sockinfo;

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::mutex;

using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::create_directory;

using json = nlohmann::ordered_json;

class button;

class profile {
public:
	int	columns		 = 6;
	int	rows		 = 4;
	int	current_page = 0;
	vector<button> buttons;
};

class id {
public:
	string ID;
	string nickname;
	string config_file;
	sockinfo sock;
	json config;
	vector<profile> profiles;
	mutex lock;
	int current_profile = 0;
	id() {}
	id(string in_ID) : ID(in_ID) {};
	id(const id& ID) {
		this->ID = ID.ID;
		this->current_profile = ID.current_profile;
		this->nickname = ID.nickname;
		this->sock = ID.sock;
		this->config_file = ID.config_file;
		this->config = ID.config;
		this->profiles = ID.profiles;
	}
	id(const id* ID) {
		this->ID = ID->ID;
		this->current_profile = ID->current_profile;
		this->nickname = ID->nickname;
		this->sock = ID->sock;
		this->config_file = ID->config_file;
		this->config = ID->config;
		this->profiles = ID->profiles;
	}
};

extern unordered_map<string, id> ids;

extern mutex ids_lock;
extern bool button_cleared;

extern int configure_id(id& ID);

extern void clear_button(int profile, int page, int button);
extern void reconfigure(id& ID);
extern void write_config(id& ID);
