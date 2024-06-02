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
#define NX_SOCKET_ERROR INVALID_SOCKET

#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH

#define close(sockfd) closesocket(sockfd)
#else
typedef int socket_t;
#define NX_SOCKET_ERROR -1

#define SD_RECEIVE SHUT_RD
#define SD_SEND    SHUT_WR
#define SD_BOTH    SHUT_RDWR

#define closesocket(sockfd) close(sockfd)
#endif

#define nx_sock_close(sockfd) close(sockfd)

// shutdown() flags
#define NXS_SHUT_R  SHUT_RD
#define NXS_SHUT_W  SHUT_WR
#define NXS_SHUT_RW SHUT_RDWR

inline socket_t nx_sock_init(int domain, int type, int protocol) {
    #ifdef _WIN32
    WSADATA wsadata;
    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res) {
        LPSTR err_msg;
        DWORD fmt_res = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                      NULL, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&err_msg, 0, NULL);
        fprintf(stderr, "WSAStartup failed: %s\n", err_msg);
        LocalFree(err_msg);
        return res;
    }
    #endif
    return socket(domain, type, protocol);
}

#endif
