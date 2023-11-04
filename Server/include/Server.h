#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include "Config.h"

using std::string;
using std::vector;

struct sockinfo {
	WSADATA wsaData;
	int iResult = 1;
	int iSendResult = 1;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;
	struct addrinfo hints;
	struct addrinfo* result = NULL;
};

extern int server_init();

extern int setup_sock(int& iResult,
					  WSADATA& wsaData,
					  SOCKET& ListenSocket,
					  SOCKET& ClientSocket,
					  struct addrinfo hints,
					  struct addrinfo* result,
					  int socktype);
extern int accept_socket(sockinfo& sock, char* idbuf, char* sock_id, bool& is_valid);

extern sockinfo sock_init(int type);

extern bool is_id(string message);

extern void parse_message(string message);
extern void comm_comm(sockinfo commsock);
extern void conf_comm(sockinfo confsock);
extern void reconfigure(id& id, sockinfo& sockinfo);
extern void begin_accept_cycle(sockinfo& commsock, sockinfo& confsock);
