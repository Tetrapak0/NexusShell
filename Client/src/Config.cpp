#include "../include/Config.h"
#include "../include/Header.h"
#include "../include/Client.h"

struct passwd* pw = getpwuid(getuid());

char* homedir = pw->pw_dir;

bool have_ip  = false;

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
    nxsh_dir += "/.config";
    if (!exists(nxsh_dir)) create_directory(nxsh_dir);
    nxsh_dir += "/NexusShell";
    string uuid_path = nxsh_dir + "/UUID";
    if (!exists(uuid_path)) {
        if (!exists(nxsh_dir)) create_directory(nxsh_dir);
        const string id = id_gen();
        ofstream uuid_file(uuid_path);
        uuid_file << id;
        uuid_file.close();
        return id;
    } else {
        ifstream uuid_file(uuid_path);
        char idbuf[19];
        uuid_file.getline(idbuf, 19);
        const string id(idbuf);
        uuid_file.close();
        return id;
    }
}

bool rw_ipstore() {
    string nxsh_config(homedir);
    nxsh_config += "/.config";
    if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/NexusShell";
    if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/ipstore";
    if (exists(nxsh_config)) {
        ifstream ip_reader(nxsh_config);
        string read_ip((istreambuf_iterator<char>(ip_reader)),
                               (istreambuf_iterator<char>()));
        ip_reader.close();
        if (!read_ip.empty()) {
            strncpy(ip_address, read_ip.c_str(), read_ip.length());
            if (!kill) failed = false;
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
    nxsh_config += "/.config";
    if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/NexusShell";
    nxsh_config += "/config.json";
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

void clear_config() {
    cerr << "here\n";
    string nxsh_config(homedir);
    nxsh_config += "/.config";
    if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/NexusShell";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/config.json";
    ofstream writer(nxsh_config);
    profiles.clear();
    profile profile1;
    for (int i = 1; i <= profile1.columns * profile1.rows; ++i) {
        button button1;
        profile1.buttons.push_back(button1);
    }
    profiles.push_back(profile1);
    writer.close();
}

void write_config(string recvd_config) {
	string nxsh_config(homedir);
    nxsh_config += "/.config";
    if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/NexusShell";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
    nxsh_config += "/config.json";
    config = json::parse(recvd_config);
	ofstream writer(nxsh_config);
	writer << config.dump(4);
	writer.close();
    set_properties();
}

void set_properties() {
    reconfiguring = true;
    profiles.clear();
    if (config[config.begin().key()].contains("profiles")) {
        int profile_count = 0;
        for (auto& profile : config[config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
        for (int i = 0; i < profile_count; ++i) {
            profile profile1;
            json profile_store = config[config.begin().key()]["profiles"][to_string(i)];
            if (profile_store.contains("columns")) profile1.columns = std::stoi(profile_store["columns"].get<string>());
            if (profile_store.contains("rows"))    profile1.rows    = std::stoi(profile_store["rows"].get<string>());
            if (profile_store.contains("pages")) {
                int page_count = 0;
                for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
                for (int j = 0; j < page_count; ++j) {
                    json page_store = profile_store["pages"][to_string(j)];
                    if (page_store.contains("buttons")) {
                        for (int k = 0; k < profile1.columns * profile1.rows; ++k) {
                            button button1;
                            if (page_store["buttons"].contains(to_string(k))) {
                                json button_store = page_store["buttons"][to_string(k)];
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
                    }
                }
            }
            profiles.push_back(profile1);
        }
    }
    reconfiguring = false;
    cerr << "configured\n";
}
