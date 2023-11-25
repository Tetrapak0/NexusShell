#pragma once

#define ID_LENGTH 18
#define SHORTCUT_PREFIX "SH"

#include <winsock2.h>
#include <ws2tcpip.h>

using std::string;
using std::vector;

class id;

struct sockinfo {
	WSADATA wsaData;
	int iResult = 1;
	int iSendResult = 1;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;
	struct addrinfo hints;
	struct addrinfo* result = NULL;
};

extern unordered_map<string, sockinfo> sock_map;

extern int server_init();

extern sockinfo sock_init();

extern int setup_sock(sockinfo& si);
extern int accept_socket(sockinfo& sock, char* idbuf, bool& is_valid);

extern bool is_id(string message);

extern void begin_accept_cycle(sockinfo& sock);
extern void comm();
extern void parse_message(string& message, id& ID);
