#include "../include/Header.h"

struct passwd* pw = getpwuid(getuid());
struct timeval timeout;

bool failed;
bool connected;
bool disconnected_modal;

int sock;
int bytesReceived;
int sendRes;

char ip_address[16];
char* homedir = pw->pw_dir;

int client_init() {
    const string pad_id = rw_UUID_file();
    timeout.tv_sec  = 5;
    timeout.tv_usec = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {done = true; return 1;}
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout,  sizeof(timeout)) == -1) {done = true; return 1;}
    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout,  sizeof(timeout)) == -1) {done = true; return 1;}

    failed             = true;
    connected          = false;
    disconnected_modal = false;

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(27015);
    do {
        if (!failed) { // TODO: auto connect
            inet_pton(AF_INET, ip_address, &hint.sin_addr);
            int connectRes = connect(sock, (struct sockaddr*)&hint, sizeof(hint));
            cerr << "connectRes: " << connectRes << "\n";
            if (connectRes == -1) {failed = true;}
            else                  connected = true;
        }
    } while (!connected);
    should_wait = true;
    char buf[128];
    sendRes = send(sock, pad_id.c_str(), pad_id.size() + 1, 0);
    do {
        memset(buf, 0, 128);
        bytesReceived = recv(sock, buf, 128, 0);
        should_wait = false;
        if (bytesReceived == -1) break;
        else cout << string(buf, bytesReceived) << "\n";
        should_wait = true;
    } while (bytesReceived > 0 || sendRes > 0 || !done);
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

const string rw_UUID_file() {
    string shp_dir(homedir);
    shp_dir += "/.config/ShortPad";
    string uuid_path = shp_dir + "/UUID";
    if (!exists(uuid_path)) {
        if (!exists(shp_dir)) create_directory(shp_dir);
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

void write_config(vector<string> args, int arg_size) {
	string shp_dir(homedir);
	shp_dir += "/.config/ShortPad";
	if (!exists(shp_dir)) create_directory(shp_dir);
	string shp_config = shp_dir + "/config.json";
	json to_write;
	if (exists(shp_config)) {
		ifstream parse_config(shp_config);
		if (parse_config.peek() != std::ifstream::traits_type::eof()) {
			to_write = json::parse(parse_config);
		}
		parse_config.close();
	}

	for (int i = arg_size - 1; i >= 0; --i) {
		if (!args[i].empty()) {
			json* temp = &to_write;
			for (int k = 0; k < i; ++k) temp = &(*temp)[args[k]];
			if (i > 0)					*temp = (*temp)[args[i - 1]] = args[i];
			else						*temp = args[0];
			break;
		}
	}

	ofstream write_config(shp_config);
	write_config << to_write.dump(4);
	write_config.close();
}
