#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/Client.h"
#include "../include/GUI.h"

struct timeval timeout;

bool failed             = true;
bool connected          = false;
bool kill               = false;
bool have_ip            = false;
bool have_port          = false;

int sock;
int bytesReceived;
int send_result;
int port = 27015;
int prev_port = port;

char ip_address[16];

string kill_reason;

int client_init() {
    const string ID = rw_UUID();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {done = true; return 1;}

    timeout.tv_sec = 5;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        LOG("setsockopt failed"); done = true; return 1;
    }
    // TODO: multiple saved servers. select which one to connect to
    have_ip = read_ipstore();
    have_port = read_portstore();

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    check_config();
    do {
        if (!failed) {
            if (prev_port != port) {
                write_ipstore();
                write_portstore();
                prev_port = port;
                close(sock);
            	LOG("------------REBOOTING SOCK------------");
                return client_init();
            }
            hint.sin_port = htons(port);
            VARLOG("port", port);
            VARLOG("hint.sin_port", hint.sin_port);
            VARLOG("IP address", ip_address);
            inet_pton(AF_INET, ip_address, &hint.sin_addr);
            int connectRes = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
            VARLOG("connectRes", connectRes);
            if (connectRes == -1) failed = true;
            else                  connected = true;
        }
        if (done) return 0;
    } while (!connected);

    if (!have_ip) write_ipstore();
    if (!have_port) write_portstore();

    timeout.tv_sec = 0;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        LOG("setsockopt failed"); done = true; return 1;
    }

    disconnected_modal = false;

    string message;
    char* buf = nullptr;
    try {
		buf = new char[1024 * 256];
	} catch (std::bad_alloc) {
		LOG("Failed to allocate buffer.");
		error_dialog(1, "Failed to allocate buffer.\n");
		exit(-1);
	}
	if (buf == nullptr) {
		LOG("Failed to allocate buffer.");
		error_dialog(1, "Failed to allocate buffer.\n");
		exit(-1);
	}
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
	LOG("------------REBOOTING SOCK------------");
    if (!done) return client_init();
    return 0;
}
