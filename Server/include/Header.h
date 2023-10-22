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
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <Windows.h>
#include <WinUser.h>
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
using std::ofstream;
using std::ifstream;
using std::stringstream;
using std::istreambuf_iterator;

using std::filesystem::exists;
using std::filesystem::create_directory;

using json = nlohmann::json;

extern bool done;

extern int connected_devices;

class id {
public:
	string ID;
	id(string in_ID);
	bool operator==(const id& other) const;
};

extern vector<id> ids;

extern int server_init();

extern int setup_sock(int& iResult, 
					  WSADATA& wsaData,
		   			  SOCKET& ListenSocket,
					  SOCKET& ClientSocket,
					  struct addrinfo hints,
					  struct addrinfo* result);
extern void ping(bool restart, int iSendResult, int iResult, SOCKET ClientSocket);
extern void parse_command(string message);
extern void write_config(vector<string> args, int arg_size);
extern void prepare_json();