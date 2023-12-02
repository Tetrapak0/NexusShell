#pragma once

#define CURRENT_PROFILE profiles[current_profile]
#define CURRENT_BUTTOM_M1_LOOP CURRENT_PROFILE.buttons[i-1]

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>

using std::string;

extern int sock;
extern int send_result;
extern int bytes_received;
extern int port;
extern int prev_port;

extern bool failed;
extern bool connected;
extern bool kill;
extern bool have_ip;
extern bool have_port;

extern char ip_address[16];

extern string kill_reason;

extern int client_init();

extern void comm();
