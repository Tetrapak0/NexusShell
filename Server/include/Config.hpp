#pragma once

#define CURRENT_SHELL           shells[selected_shell]
#define CURRENT_PROFILE         CURRENT_SHELL.profiles[CURRENT_SHELL.current_profile]
#define CURRENT_PROFILE_SELF    shell.profiles[shell.current_profile]

#define CURRENT_BUTTON          CURRENT_PROFILE.buttons[index] // TODO: Change to actual page
#define CURRENT_BUTTON_LOOP     CURRENT_PROFILE.buttons[i]

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <mutex>

#include "Server.hpp"
#include "GUI.hpp"

#include "../../external/jsonhpp/json.hpp"

//#include "../../external/zlib/include/zlib.h"
//#include "../../external/zlib/include/zconf.h"

//#pragma comment(lib, "../external/zlib/bin/zlib.lib")

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::mutex;
using std::unordered_map;

using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::create_directory; // TODO: Schedule for removal. on client too.
using std::filesystem::create_directories;

using json = nlohmann::ordered_json;

class Button;

class Profile {
public:
	int columns		 = 6;
	int rows		 = 4;
    int size         = columns * rows;
	int current_page = 0;
	vector<Button> buttons;
};

class Shell {
public:
	string ID;
	string nickname;
	string config_path;
	socket_t client_socket;
    int res = 1, send_res = 1;
	json config;
	vector<Profile> profiles;
	mutex lock;
	int current_profile = 0;
	Shell() = default;
	Shell(const Shell& shell) {
		this->ID = shell.ID;
		this->current_profile = shell.current_profile;
		this->nickname = shell.nickname;
		this->client_socket = shell.client_socket;
		this->config_path = shell.config_path;
		this->config = shell.config;
		this->profiles = shell.profiles;
	}
	Shell(const Shell* shell) {
		this->ID = shell->ID;
		this->current_profile = shell->current_profile;
		this->nickname = shell->nickname;
        this->client_socket = shell->client_socket;
		this->config_path = shell->config_path;
		this->config = shell->config;
		this->profiles = shell->profiles;
	}
};

extern unordered_map<string, Shell> shells;

extern mutex shells_lock;
extern bool button_cleared;

extern int configure_shell(Shell& shell);

extern void clear_button(int profile, int page, int button);
extern void reconfigure_shell(Shell& shell);
extern void config_write(Shell& shell);
