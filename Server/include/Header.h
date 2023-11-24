#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <unordered_map>

#include <Windows.h>
#include <WinUser.h>
#include <shellapi.h>

#include "../../json.hpp"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "user32.lib")

using std::cin;
using std::cerr;

using std::string;
using std::vector;
using std::thread;
using std::unordered_map;

using std::to_string;

extern bool done;
extern bool button_cleared;

extern int selected_id;
extern int connected_devices;