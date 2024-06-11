#include "../include/Config.hpp"
#include "../include/Header.hpp"
#include "../include/Client.hpp"

string nxsh_dir;

json config;

atomic<bool> reconfiguring = false;

const string UUID_gen() {
    const uint64_t uuid_min = 100000000000000000;
    const uint64_t uuid_max = 999999999999999999;

    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_int_distribution<uint64_t> distr(uuid_min, uuid_max);

    return to_string(distr(generator));
}

const string UUID_rw() {
    string nxsh_dir = string(homedir) + "/.config/NexusShell";
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
        const string id = UUID_gen();
        ofstream writer(nxsh_file);
        writer << id;
        writer.close();
        return id;
    }
}

bool ipstore_read() {
    string nxsh_config = string(homedir) + "/.config/NexusShell/ipstore";
    ifstream reader(nxsh_config);
    if (exists(nxsh_config)) {
        if (reader.peek() != ifstream::traits_type::eof()) {
            reader.getline(ip_address, 16);
            reader.close();
            if (!kill) failed = false;
            return true;
        }
        reader.close();
    }
    return false;
}

void ipstore_write() {
    string nxsh_config = string(homedir) + "/.config/NexusShell";
    create_directories(nxsh_config);
    nxsh_config += "/ipstore";
    ofstream writer(nxsh_config);
    writer << ip_address;
    writer.close();
}

void ipstore_remove() {
    string nxsh_config = string(homedir) + "/.config/NexusShell/ipstore";
    if (exists(nxsh_config)) std::filesystem::remove(nxsh_config);
}

bool portstore_read() {
    string nxsh_config = string(homedir) + "/.config/NexusShell/portstore";
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

void portstore_write() {
    string nxsh_config = string(homedir) + "/.config/NexusShell";
    create_directories(nxsh_config);
    nxsh_config += "/portstore";
    ofstream writer(nxsh_config);
    writer << port;
    writer.close();
}

void portstore_remove() {
    string nxsh_config = string(homedir) + "/.config/NexusShell/portstore";
    if (exists(nxsh_config)) std::filesystem::remove(nxsh_config);
}

void config_check() {
    string nxsh_dir = string(homedir) + "/.config/NexusShell";
    string nxsh_file = nxsh_dir + "/config.json";
    if (exists(nxsh_file)) {
        ifstream reader(nxsh_file);
        if (reader.peek() != ifstream::traits_type::eof()) {
            config = json::parse(reader);
            config_read();
        }
        reader.close();
    }
}

void config_remove() {
    string nxsh_config = string(homedir) + "/.config/NexusShell";
    create_directories(nxsh_config);
    nxsh_config += "/config.json";
    profiles.clear();
    Profile profile;
    for (int i = 0; i < profile.size; ++i) profile.buttons.push_back(Button());
    profiles.push_back(profile);
    ofstream writer(nxsh_config);
    writer.close();
}

void config_write(string recvd_config) {
    config = json::parse(recvd_config);
	string nxsh_config = string(homedir) + "/.config/NexusShell";
	create_directories(nxsh_config);
    nxsh_config += "/config.json";
	ofstream writer(nxsh_config);
	writer << config.dump(4);
	writer.close();
    config_read();
}

void config_read() {
    reconfiguring.exchange(true);
    profiles.clear();
    if (config[config.begin().key()].contains("profiles")) {
        int profile_count = 0;
        for (auto& profile : config[config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
        for (int i = 0; i < profile_count; ++i) {
            Profile profile;
            json& profile_store = config[config.begin().key()]["profiles"][to_string(i)];

            if (profile_store.contains("columns")) 
                profile.columns = stoi(profile_store["columns"].get<string>());
            if (profile_store.contains("rows")) 
                profile.rows = stoi(profile_store["rows"].get<string>());
            profile.size = profile.columns * profile.rows;

            if (profile_store.contains("pages")) {
                int page_count = 0;
                for (auto& page : profile_store["pages"]) if (page.is_object()) page_count++;
                for (int j = 0; j < page_count; ++j) {
                    json& page_store = profile_store["pages"][to_string(j)];
                    if (page_store.contains("buttons")) {
                        for (int k = 0; k < profile.columns * profile.rows; ++k) {
                            Button button;
                            if (page_store["buttons"].contains(to_string(k))) {
                                json& button_store = page_store["buttons"][to_string(k)];
                                if (button_store.contains("label")) button.label = button_store["label"];
                                if (button_store.contains("has default label")) {
							        if (button_store["has default label"] == "1") button.default_label = true;
							        else if (button_store["has default label"] == "0") button.default_label = false;
							        else if ((button.label == "")) button.default_label = true;
							        else button.default_label = false;
						        }
                                if (button_store.contains("action")) button.action = button_store["action"];
                            }
                            profile.buttons.push_back(button);
                        }
                    } else for (int k = 0; k < profile.size; ++k) profile.buttons.push_back(Button());
                }
            } else for (int k = 0; k < profile.size; ++k) profile.buttons.push_back(Button());
            profiles.push_back(profile);
        }
    } else {
		Profile profile;
		for (int i = 0; i < profile.size; ++i)
			profile.buttons.push_back(Button());
		profiles.push_back(profile);
	}
    reconfiguring.exchange(false);
    LOG("configured");
}

