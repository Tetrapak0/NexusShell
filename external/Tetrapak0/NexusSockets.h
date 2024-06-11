#ifndef __NX_SOCK_H__
#define __NX_SOCK_H__

#include <stdio.h>

#ifdef _WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <netdb.h>

#include <errno.h>
#endif

#ifdef _WIN32
typedef SOCKET socket_t;
typedef int    socklen_t;

#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH

#define NX_INVALID_SOCKET INVALID_SOCKET

#define nx_sock_close(sockfd) closesocket(sockfd)
#define cleanup() WSACleanup()
#else
typedef int socket_t;

#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR
#define SD_BOTH    SHUT_RDWR

#define NX_INVALID_SOCKET -1

#define nx_sock_close(sockfd) close(sockfd)
#define cleanup() {}
#endif

#define NX_SOCKET_ERROR -1

// shutdown() flags
#define NXS_SHUT_R  SHUT_RD
#define NXS_SHUT_W  SHUT_WR
#define NXS_SHUT_RW SHUT_RDWR

#endif
