#include "../include/Header.hpp"
#include "../include/Config.hpp"
#include "../include/Client.hpp"
#include "../include/GUI.hpp"

struct timeval timeout;

bool failed    = true;
bool connected = false;
bool kill      = false;
bool have_ip   = false;
bool have_port = false;

socket_t sock;

int bytes_received;
int send_result;

int port      = 27015;
int prev_port = port;

char ip_address[16];

string kill_reason;

int client_init() {
    const string ID = UUID_rw();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == NX_SOCKET_ERROR) {
        done = true;
        return -1;
    }
    // TODO: Check errno
    timeout.tv_sec = 5;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) ||
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout))) {
        LOG("setsockopt failed");
        done = true;
        return 1;
    }
    // TODO: multiple saved servers. select which one to connect to
    have_ip = ipstore_read();
    have_port = portstore_read();

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    config_check();
    do {
        if (done) return 0;
        if (!failed) {
            if (prev_port != port) {
                ipstore_write();
                portstore_write();
                prev_port = port;
                nx_sock_close(sock);
            	LOG("------------REBOOTING SOCKET------------");
                return client_init();
            }
            hint.sin_port = htons(port);
            VARLOG("port", port);
            VARLOG("hint.sin_port", hint.sin_port);
            VARLOG("IP address", ip_address);
            inet_pton(AF_INET, ip_address, &hint.sin_addr);
            int connect_res = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
            VARLOG("connect_res", connect_res);
            if (connect_res == -1) failed = true;
            else                   connected = true;
        }
    } while (!connected);

    if (!have_ip)   ipstore_write();
    if (!have_port) portstore_write();

    timeout.tv_sec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout,  sizeof(timeout)) ||
        setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout,  sizeof(timeout))) {
        LOG("setsockopt failed");
        done = true;
        return 1;
    }

    disconnected_modal = false;

    string message;
    char* buf = nullptr;
    try {
		buf = new char[1024 * 256];
	} catch (std::bad_alloc) {
		LOG("Failed to allocate memory for message buffer.");
		error_dialog(1, "Failed to allocate memory for message buffer.\n");
		exit(-1);
	}
	if (!buf) {
		LOG("Failed to allocate memory for message buffer.");
		error_dialog(1, "Failed to allocate memory for message buffer.\n");
		exit(-1);
	}
    send_result = send(sock, ID.c_str(), ID.length() + 1, 0);
    do {
        memset(buf, 0, 1024 * 256);
        bytes_received = recv(sock, buf, 1024 * 256, 0);
        message = buf;
        if (bytes_received < 1) break;
        else {
            cerr << message << "\n";
            if (message.substr(0, 3) == "cfg")
                config_write(message.substr(3, message.length()));
            else if (message.substr(0, 4) == "kill") {
                kill = true;
                kill_reason = message.substr(4, message.length());
            }
        }
    } while (bytes_received > 0 || send_result > 0 || !done);
    delete[] buf;
    nx_sock_close(sock);
    connected = false;
    if (!kill) disconnected_modal = true;
    failed = true;
	LOG("------------REBOOTING SOCKET------------");
    if (!done) return client_init();
    return 0;
}

