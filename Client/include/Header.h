#pragma once

#define SHORTCUT_PREFIX "SH"

#define LOG(x) cerr << x << "\n";
#define VAR_LOG(x, y) cerr << x << ": " << y << "\n";

#include <iostream>
#include <thread>
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

extern bool failed; // TODO: Move to specific headers
extern bool failed_backup;
extern bool done;
extern bool disconnected_modal;

extern char ip_address[16];
