#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define ID_LENGTH 18
#define SHORTCUT_PREFIX "SH"

#define CURRENT_ID_AND_BUTTON ids[selected_id].profiles[0].buttons[index] // TODO: Change this to actual profile and page
#define CURRENT_ID			  ids[selected_id]

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

using std::to_string;
using std::filesystem::exists;
using std::filesystem::create_directory;

using json = nlohmann::ordered_json;

extern bool done;
extern bool send_config;

extern int connected_devices;

class button {
public:
	string label = "";
	string label_backup = label;
	bool   default_label = true;
	enum   types {File, URL, Command,};
	types  type = File;
	string action;
};

class profile {
public: 
	int	   columns		   = 6;
	int	   rows			   = 4;
	int	   current_page    = 0;
	vector<button> buttons;
};

class id {
public:
	string ID;
	vector<profile> profiles;
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

extern void configure_id(id* id);
extern void parse_message(string message);
extern void write_config(vector<string> args, int arg_size);
