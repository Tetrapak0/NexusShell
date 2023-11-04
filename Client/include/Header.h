#pragma once

#define SHORTCUT_PREFIX "SH"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>

#include <unistd.h>
#include <pwd.h>

#include "../../json.hpp"

using std::cout;
using std::cin;
using std::cerr;

using std::string;
using std::thread;
using std::vector;

using std::to_string;

extern bool failed;
extern bool failed_backup;
extern bool has_commsock;
extern bool has_confsock;
extern bool done;
extern bool disconnected_modal;

extern char ip_address[16];
