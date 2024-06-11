#pragma once

#define	LOG(x)			cerr << x << "\n"
#define	LOGVAR(x, y)	cerr << x << ": " << y << "\n"

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
#define NX_PATH_GET() string(getenv("APPDATA")) + "/NexusSHell/"
#else
#define NX_PATH_GET() string(getenv("HOME")) + "/.config/NexusShell/"
#endif

#define NXSH_VERSION "Alpha 1.5" // URGENT: Update each version

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <unordered_map>

#ifdef _WIN32
#include <Windows.h>
#include <WinUser.h>
#include <shellapi.h>

#pragma comment(lib, "user32.lib")
#endif

using std::cin;
using std::cerr;

using std::string;
using std::vector;
using std::thread;
using std::unordered_map;
using std::wstring;

using std::to_string;

extern bool done;
