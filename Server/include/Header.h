#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define ID_LENGTH 18
#define SHORTCUT_PREFIX "SH"

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <iomanip>
#include <Windows.h>
#include <WinUser.h>
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../../json.hpp"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "user32.lib")

using std::cin;
using std::cerr;

using std::string;
using std::vector;
using std::thread;
using std::stringstream;

using std::to_string;

extern bool done;
extern bool button_cleared;

extern int selected_id;
extern int connected_devices;

extern int server_init();

extern int setup_sock(int& iResult, 
					  WSADATA& wsaData,
		   			  SOCKET& ListenSocket,
					  SOCKET& ClientSocket,
					  struct addrinfo hints,
					  struct addrinfo* result);

extern void parse_message(string message);
