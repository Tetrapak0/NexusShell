#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/Client.h"

struct timeval timeout;

bool failed             = true;
bool connected          = false;
bool disconnected_modal = false;
bool kill               = false;

int sock;
int bytesReceived;
int send_result;

char ip_address[16];

string kill_reason;

json config;

int client_init() {
    const string ID = rw_UUID();

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
    // if previous MAYBETODO is implemented, add a check_config here with current connected ip as param

    timeout.tv_sec = 0;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        cerr << "setsockopt failed\n"; done = true; return 1;
    }

    disconnected_modal = false;
    string message;
    char* buf = new char[1024 * 256];
    send_result = send(sock, ID.c_str(), ID.length() + 1, 0);
    do {
        memset(buf, 0, 1024 * 256);
        bytesReceived = recv(sock, buf, 1024 * 256, 0);
        message = buf;
        if (bytesReceived == -1 || bytesReceived == 0) break;
        else {
            cerr << message << "\n";
            if (message.substr(0, 3) == "cfg") write_config(message.substr(3, message.length()));
            else if (message.substr(0, 4) == "kill") {
                kill = true;
                kill_reason = message.substr(4, message.length());
            }
        }
    } while (bytesReceived > 0 || send_result > 0 || !done);
    delete[] buf;
    close(sock);
    connected = false;
    if (!kill) disconnected_modal = true;
    failed = true;
	cerr << "------------REBOOTING SOCK------------\n";
    if (!done) return client_init();
    return 0;
}
