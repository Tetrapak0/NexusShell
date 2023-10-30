#include "../include/Header.h"

struct passwd* pw = getpwuid(getuid());
struct timeval timeout;

bool failed;
bool connected          = false;
bool disconnected_modal = false;
bool have_ip            = false;

int sock;
int bytesReceived;
int sendRes;
int next_msgs_are_config = 0;

char ip_address[16];
char* homedir = pw->pw_dir;

json config;

int client_init() {
    const string pad_id = rw_UUID();

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {done = true; return 1;}

    timeout.tv_sec  = 5;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        done = true; cerr << "setsockopt failed\n"; return 1;
    }

    failed             = true;
    connected          = false;

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(27015);
    have_ip = rw_ipstore();
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
    check_config();

    timeout.tv_sec = 0;
    if ((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) ||
         setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout))  == -1)) {
        cerr << "setsockopt failed\n"; done = true; return 1;
    }

    disconnected_modal = false;
    string message;
    char* buf = (char*)malloc(sizeof(char) * (1024 * 256));
    sendRes = send(sock, pad_id.c_str(), pad_id.size() + 1, 0);
    do {
        memset(buf, 0, 1024 * 256);
        bytesReceived = recv(sock, buf, 1024 * 256, 0);
        message = buf;
        if (bytesReceived == -1 || bytesReceived == 0) break;
        else {
            cerr << buf << "\n";
            if (message.length() > 4) {
                if (message.substr(0, 3) == "cfg") {
                    write_config(message.substr(3, message.length()));
                }
            }
        }
    } while (bytesReceived > 0 || sendRes > 0 || !done);
    free(buf);
    close(sock);
    connected = false;
    disconnected_modal = true;
	cerr << "------------REBOOTING SOCK------------\n";
    if (!done) return client_init();
    return 0;
}

const string id_gen() {
    const unsigned long long min = 100000000000000000;
    const unsigned long long max = 999999999999999999;

    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_int_distribution<unsigned long long> distr(min, max);

    return to_string(distr(generator));
}

const string rw_UUID() {
    string nxsh_dir(homedir);
    nxsh_dir += "/.config/NexusShell";
    string uuid_path = nxsh_dir + "/UUID";
    if (!exists(uuid_path)) {
        if (!exists(nxsh_dir)) create_directory(nxsh_dir);
        const string id = id_gen();
        ofstream uuid_file(uuid_path);
        uuid_file << id;
        return id;
    } else {
        ifstream uuid_file(uuid_path);
        const string id((istreambuf_iterator<char>(uuid_file)), 
                                (istreambuf_iterator<char>()));
        return id;
    }
}

bool rw_ipstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell";
    if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/ipstore";
    if (exists(nxsh_config)) {
        ifstream ip_reader(nxsh_config);
        string read_ip((istreambuf_iterator<char>(ip_reader)),
                               (istreambuf_iterator<char>()));
        ip_reader.close();
        if (!read_ip.empty()) {
            strncpy(ip_address, read_ip.c_str(), read_ip.length());
            failed = false;
            return true;
        }
    }
    ofstream ip_hoarder(nxsh_config);
    ip_hoarder << ip_address;
    ip_hoarder.close();
    return false;
}

void remove_ipstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell/ipstore";
    if (exists(nxsh_config)) std::filesystem::remove(nxsh_config);
}

void check_config() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell/config.json";
    if (exists(nxsh_config)) {
        ifstream reader(nxsh_config);
        if (reader.peek() != ifstream::traits_type::eof()) {
            try {
                config = json::parse(reader);
                set_properties();
            } catch (...) {cerr << "Unable to parse config\n";}
        }
        reader.close();
    }
}

void write_config(string recvd_config) {
	string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/config.json";
    config = json::parse(recvd_config);
	ofstream writer(nxsh_config);
	writer << config.dump(4);
	writer.close();
    set_properties();
}
