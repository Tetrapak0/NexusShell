#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct sockinfo {
    bool connected;
    int sock;
    int send_result;
    int bytes_received;
    struct sockaddr_in hint;
};

extern sockinfo commsock;
extern sockinfo confsock;

extern int client_init();

extern sockinfo sock_init(int sock_type);

extern void comm_comm();
extern void conf_comm();
extern void sock_connect(sockinfo& sock, string shell_id);

