#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/Client.h"

bool failed             = true;
bool disconnected_modal = false;
bool has_commsock       = false;
bool has_confsock       = false;

char ip_address[16];

json config;

sockinfo commsock;
sockinfo confsock;

int client_init() {
    const string shell_id = rw_UUID();
    commsock = sock_init(0);
    confsock = sock_init(1);
    if (commsock.sock == -1 || confsock.sock == -1) {
        done = true; // TODO: Add error message;
        return -1;
    }
    have_ip = rw_ipstore();
    check_config();                          // MAYBETODO: Multiple computers -- different configs
    sock_connect(commsock, shell_id);
    has_commsock = true;
    if (!have_ip) rw_ipstore();              // if previous MAYBETODO is implemented, add a check_config here with current connected ip as parameter
    sock_connect(confsock, shell_id);
    has_confsock = true;
    thread configurator(conf_comm);
    comm_comm();
    configurator.join();
    close(commsock.sock);
    close(confsock.sock);
    cerr << "------------REBOOTING SOCK------------\n";
    if (!done) return client_init();
    return 0;
}

sockinfo sock_init(int sock_type) {  // 0 = commsock, 1 = confsock
    sockinfo co_sock;
    co_sock.sock = socket(AF_INET, SOCK_STREAM, 0);
    switch (sock_type) {
        case 0: co_sock.hint.sin_port = htons(27015); break;
        case 1: co_sock.hint.sin_port = htons(27016); break;
    }
    co_sock.hint.sin_family = AF_INET;
    return co_sock;
}

void sock_connect(sockinfo& sock, string shell_id) {  // TODO: modify setup window aswell for custom ports;
    do {
        if (!failed) {
            inet_pton(AF_INET, ip_address, &sock.hint.sin_addr);
            int connectRes = connect(sock.sock, (struct sockaddr*)&sock.hint, sizeof(sock.hint));
            cerr << "connectRes: " << connectRes << "\n";
            if (connectRes == -1) failed = true;
            else                  sock.connected = true;
        }
        if (done) return;
    } while (!sock.connected);
    sock.send_result = send(sock.sock, shell_id.c_str(), shell_id.length() + 1, 0);
}

void comm_comm() {
    string message;
    char buf[19] ;
    do {
        memset(buf, 0, 19);
        commsock.bytes_received = recv(commsock.sock, buf, 19, 0);
        message = buf;
        if (commsock.bytes_received == -1 || commsock.bytes_received == 0) break;
        else cerr << message << "\n";
        cerr << "br: " << commsock.bytes_received << "\n";
        cerr << "msg: " << message << "\n";
    } while (commsock.bytes_received > 0 && commsock.send_result > 0 && !done && has_confsock);
    commsock.connected = false;
    has_commsock       = false;
}

void conf_comm() {
    string message;
    char* buf = (char*)malloc(sizeof(char) * (1024 * 256));
    do {
        memset(buf, 0, 1024 * 256);
        confsock.bytes_received = recv(confsock.sock, buf, 1024 * 256, 0);
        message = buf;
        if (confsock.bytes_received == -1 || confsock.bytes_received == 0) break;
        else {
            cerr << message << "\n";
            if (message.substr(0, 3) == "cfg") write_config(message.substr(3, message.length()));
        }
        cerr << "br: " << confsock.bytes_received << "\n";
    } while (confsock.bytes_received > 0 && confsock.send_result > 0 && !done && has_commsock);
    free(buf);
    confsock.connected = false;
    has_confsock       = false;
    return;
}
