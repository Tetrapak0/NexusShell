#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <mutex>

#include "../../external/jsonhpp/json.hpp"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;
using std::atomic;

using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::create_directory;
using std::filesystem::create_directories;

using json = nlohmann::ordered_json;

extern json config;

extern atomic<bool> reconfiguring;

extern string nxsh_dir;

class Button {
public:
	string label = "";
    bool default_label = true;
	string action;
};

class Profile {
public: 
	int	columns = 6;
	int	rows    = 4;
    int size    = columns * rows;
	int	current_page = 0;
	vector<Button> buttons;
};

extern vector<Profile> profiles;

extern const string UUID_gen();
extern const string UUID_rw();

extern bool ipstore_read();
extern void ipstore_write();
extern void ipstore_remove();

extern bool portstore_read();
extern void portstore_write();
extern void portstore_remove();

extern void config_check();
extern void config_read();
extern void config_remove();
extern void config_write(string recvd_config);
