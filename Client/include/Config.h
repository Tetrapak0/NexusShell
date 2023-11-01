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

extern const string id_gen();
extern const string rw_UUID();

extern bool rw_ipstore();

extern void check_config();
extern void set_properties();
extern void remove_ipstore();
extern void clear_config();
extern void write_config(string recvd_config);
