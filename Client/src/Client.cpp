#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/Client.h"

struct timeval timeout;

bool failed             = true;
bool connected          = false;
bool disconnected_modal = false;

int sock;
int bytesReceived;
int send_result;

char ip_address[16];

json config;

int client_init() {
    const string pad_id = rw_UUID();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {done = true; return 1;}

    timeout.tv_sec = 5;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        done = true; cerr << "setsockopt failed\n"; return 1;
    }

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(27015);
    have_ip = rw_ipstore();
    check_config(); // MAYBETODO: Multiple computers -- different configs
    do {
        if (!failed) {
            inet_pton(AF_INET, ip_address, &hint.sin_addr);
            int connectRes = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
            cerr << "connectRes: " << connectRes << "\n";
            if (connectRes == -1) failed = true;
            else                  connected = true;
        }
        if (done) return 0;
    } while (!connected);
    if (!have_ip) rw_ipstore();
    // if previous MAYBETODO is implemented, add a check_config here with current connected ip as parameter

    timeout.tv_sec = 0;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        cerr << "setsockopt failed\n"; done = true; return 1;
    }

    disconnected_modal = false;
    string message;
    char* buf = (char*)malloc(sizeof(char) * (1024 * 256));
    send_result = send(sock, pad_id.c_str(), pad_id.size() + 1, 0);
    do {
        memset(buf, 0, 1024 * 256);
        bytesReceived = recv(sock, buf, 1024 * 256, 0);
        message = buf;
        if (bytesReceived == -1 || bytesReceived == 0) break;
        else {
            cerr << message << "\n";
            if (message.substr(0, 3) == "cfg") write_config(message.substr(3, message.length()));
        }
    } while (bytesReceived > 0 || send_result > 0 || !done);
    free(buf);
    close(sock);
    connected = false;
    disconnected_modal = true;
	cerr << "------------REBOOTING SOCK------------\n";
    if (!done) return client_init();
    return 0;
}
