#pragma once

#define SHORTCUT_PREFIX "SH"

#define LOG(x) cerr << x << "\n"
#define VARLOG(x, y) cerr << x << ": " << y << "\n"

#include <iostream>
#include <thread>
#include <string>
#include <vector>

using std::cout;
using std::cin;
using std::cerr;

using std::string;
using std::thread;
using std::vector;

using std::to_string;

extern bool done;
extern char* homedir;
