#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <atomic>

#include "../../json.hpp"

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

extern bool read_ipstore();
extern bool read_portstore();
extern void write_ipstore();
extern void write_portstore();
extern void remove_ipstore();
extern void remove_portstore();

extern void check_config();
extern void read_config();
extern void remove_config();
extern void write_config(string recvd_config);
