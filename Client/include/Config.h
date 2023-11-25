#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <random>

#include "../../json.hpp"

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

using std::filesystem::exists;
using std::filesystem::remove;
using std::filesystem::create_directory;

using json = nlohmann::ordered_json;

extern json config;

extern bool have_ip;
extern bool reconfiguring;

extern char* homedir;

class button {
public:
	string label = "";
    bool default_label = true;
	string action;
};

class profile {
public: 
	int	   columns		   = 6;
	int	   rows			   = 4;
	int	   current_page    = 0;
	vector<button> buttons;
};

extern vector<profile> profiles;

extern const string id_gen();
extern const string rw_UUID();

extern bool rw_ipstore();

extern void check_config();
extern void set_properties();
extern void remove_ipstore();
extern void clear_config();
extern void write_config(string recvd_config);
