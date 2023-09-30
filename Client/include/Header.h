#pragma once

#define SHORTCUT_PREFIX "SH"

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <random>

#include <unistd.h>
#include <pwd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../../json.hpp" // TODO: Do we need json?

using std::cout;
using std::cin;
using std::cerr;

using std::string;
using std::thread;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::istreambuf_iterator;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

using std::to_string;
using std::filesystem::create_directory;
using std::filesystem::exists;

using json = nlohmann::json;

extern bool failed;
extern bool failed_backup;
extern bool connected;
extern bool disconnected_modal;
extern bool should_wait;
extern bool done;

extern int sock;
extern int sendRes;

extern char ip_address[16];

extern const string id_gen();
extern const string rw_UUID_file();

extern int client_init();

extern void write_config(vector<string> args, int arg_size);