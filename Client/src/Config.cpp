#include "../include/Config.h"
#include "../include/Header.h"
#include "../include/Client.h"

struct passwd* pw = getpwuid(getuid());

char* homedir = pw->pw_dir;

json config;

atomic<bool> reconfiguring = false;

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
    string nxsh_file = nxsh_dir + "/UUID";
    if (exists(nxsh_file)) {
        ifstream reader(nxsh_file);
        char idbuf[19];
        reader.getline(idbuf, 19);
        const string id(idbuf);
        reader.close();
        return id;
    } else {
        create_directories(nxsh_dir);
        const string id = id_gen();
        ofstream writer(nxsh_file);
        writer << id;
        writer.close();
        return id;
    }
}

bool read_ipstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell/ipstore";
    ifstream reader(nxsh_config);
    if (exists(nxsh_config)) {
        if(reader.peek() != ifstream::traits_type::eof()) {
            reader.getline(ip_address, 16);
            reader.close();
            if (!kill) failed = false;
            return true;
        }
        reader.close();
    }
    return false;
}

void write_ipstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell";
    create_directories(nxsh_config);
    nxsh_config += "/ipstore";
    ofstream writer(nxsh_config);
    writer << ip_address;
    writer.close();
}

void remove_ipstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell/ipstore";
    if (exists(nxsh_config)) std::filesystem::remove(nxsh_config);
}

bool read_portstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell/portstore";
    if (exists(nxsh_config)) {
        ifstream reader(nxsh_config);
        if (reader.peek() != ifstream::traits_type::eof()) {
            char temp[6];
            reader.getline(temp, 6);
            reader.close();
            string tempstr = temp;
            try { port = stoi(tempstr); } 
            catch (...) { port = 27015; return false; }
            if (port > 65535) {
                port = 27015;
                ofstream writer(nxsh_config);
                writer << port;
                writer.close();
                return false;
            }
            if (!kill) failed = false;
            return true;
        }
        reader.close();
    }
    return false;
}

void write_portstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell";
    create_directories(nxsh_config);
    nxsh_config += "/portstore";
    ofstream writer(nxsh_config);
    writer << port;
    writer.close();
}

void remove_portstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell/portstore";
    if (exists(nxsh_config)) std::filesystem::remove(nxsh_config);
}

void check_config() {
    string nxsh_dir(homedir);
    nxsh_dir += "/.config/NexusShell";
    string nxsh_file = nxsh_dir + "/config.json";
    if (exists(nxsh_file)) {
        ifstream reader(nxsh_file);
        if (reader.peek() != ifstream::traits_type::eof()) {
            config = json::parse(reader);
            read_config();
        }
        reader.close();
    }
}

void remove_config() {
    string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell";
    create_directories(nxsh_config);
    nxsh_config += "/config.json";
    profiles.clear();
    profile profile1;
    for (int i = 1; i <= profile1.columns * profile1.rows; ++i)
        profile1.buttons.push_back(button());
    profiles.push_back(profile1);
    ofstream writer(nxsh_config);
    writer.close();
}

void write_config(string recvd_config) {
    config = json::parse(recvd_config);
	string nxsh_config(homedir);
    nxsh_config += "/.config/NexusShell";
	create_directories(nxsh_config);
    nxsh_config += "/config.json";
	ofstream writer(nxsh_config);
	writer << config.dump(4);
	writer.close();
    read_config();
}

void read_config() {
    reconfiguring.exchange(true);
    profiles.clear();
    if (config[config.begin().key()].contains("profiles")) {
        int profile_count = 0;
        for (auto& profile : config[config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
        for (int i = 0; i < profile_count; ++i) {
            profile profile1;
            json& profile_store = config[config.begin().key()]["profiles"][to_string(i)];
            if (profile_store.contains("columns")) profile1.columns = stoi(profile_store["columns"].get<string>());
            if (profile_store.contains("rows"))    profile1.rows    = stoi(profile_store["rows"].get<string>());
            if (profile_store.contains("pages")) {
                int page_count = 0;
                for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
                for (int j = 0; j < page_count; ++j) {
                    json& page_store = profile_store["pages"][to_string(j)];
                    if (page_store.contains("buttons")) {
                        for (int k = 0; k < profile1.columns * profile1.rows; ++k) {
                            button button1;
                            if (page_store["buttons"].contains(to_string(k))) {
                                json& button_store = page_store["buttons"][to_string(k)];
                                if (button_store.contains("label")) button1.label = button_store["label"];
                                if (button_store.contains("has default label")) {
							        if (button_store["has default label"] == "1") button1.default_label = true;
							        else if (button_store["has default label"] == "0") button1.default_label = false;
							        else if ((button1.label == "")) button1.default_label = true;
							        else button1.default_label = false;
						        }
                                if (button_store.contains("action")) button1.action = button_store["action"];
                            }
                            profile1.buttons.push_back(button1);
                        }
                    } else for (int k = 1; k <= profile1.columns * profile1.rows; ++k) profile1.buttons.push_back(button());
                }
            } else for (int k = 1; k <= profile1.columns * profile1.rows; ++k) profile1.buttons.push_back(button());
            profiles.push_back(profile1);
        }
    } else {
		profile profile1;
		for (int i = 1; i <= profile1.columns * profile1.rows; ++i)
			profile1.buttons.push_back(button());
		profiles.push_back(profile1);
	}
    reconfiguring.exchange(false);
    LOG("configured");
}
