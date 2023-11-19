#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

using std::string;

extern int sock;
extern int send_result;
extern int bytes_received;

extern bool connected;

extern int client_init();

extern void comm();
