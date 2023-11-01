#pragma once

#define SHORTCUT_PREFIX "SH"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>

#include <unistd.h>
#include <pwd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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
extern bool connected;
extern bool done;
extern bool disconnected_modal;
extern bool have_ip;
extern bool reconfiguring;

extern int sock;
extern int sendRes;

extern char ip_address[16];
extern char* homedir;

class button {
public:
	string label = "";
    bool default_label = true;
	string action;
};

class profile {
public: 
	int	   columns		   = 6;
	int	   rows			   = 4;
	int	   current_page    = 0;
	vector<button> buttons;
};

extern vector<profile> profiles;

extern int client_init();
