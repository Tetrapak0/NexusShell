#include "../include/Header.h"

int connected_devices = 0;
int config_send_stage = 0;

bool send_config = false;
bool button_cleared = false;

id::id(string in_ID) : ID(in_ID) {

}
bool id::operator==(const id& other) const {
	return ID == other.ID;
}
vector<id> ids;

int server_init() {
	WSADATA wsaData;
	int iResult;
	int iSendResult = 1;
	SOCKET ClientSocket = INVALID_SOCKET;
	SOCKET ListenSocket = INVALID_SOCKET;
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	struct addrinfo* result = NULL;

	if (setup_sock(iResult, wsaData, ListenSocket, ClientSocket, hints, result)) { done = true; return 1; }
	ClientSocket = INVALID_SOCKET;

	while (ClientSocket == INVALID_SOCKET) ClientSocket = accept(ListenSocket, NULL, NULL);
	connected_devices++;

	int recvbuflen = 262144;
	char* buffer = (char*)malloc(sizeof(char) * (1024 * 256));
	string message;
	while (!connected_devices) {}
	u_long mode = 1;
	ioctlsocket(ClientSocket, FIONBIO, &mode);
	ioctlsocket(ListenSocket, FIONBIO, &mode);
	do {
		iResult = recv(ClientSocket, buffer, recvbuflen, 0);
		if (iResult == -1 && (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINPROGRESS) && send_config)  iResult = 1;
		if (iResult > 0) {
			if (!send_config) {
				cerr << "Bytes received: " << iResult << "\n";
				message = buffer;
				cerr << "Message: " << message << "\n";
				parse_message(message);
				iSendResult = send(ClientSocket, buffer, iResult, 0);
			} else {
				string nxsh_config(getenv("USERPROFILE"));
				nxsh_config += "\\AppData\\Roaming\\NexusShell\\";
				nxsh_config += ids[0].ID;
				nxsh_config += ".json";
				if (exists(nxsh_config)) {
					ifstream reader(nxsh_config);
					json config = json::parse(reader);
					string configuration = "cfg" + config.dump();
					cerr << configuration << "\n";
					iSendResult = send(ClientSocket, configuration.c_str(), configuration.length(), 0);
				}
				send_config = false;
			}
			if (iSendResult == SOCKET_ERROR) break;
			else cerr << "Bytes sent: " << iSendResult << "\n";
		} else if (iResult == -1 && (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEINPROGRESS)) {}
		else break;
	} while (iResult > 0 || iSendResult > 0 || !done);
	free(buffer);
	iResult = shutdown(ClientSocket, SD_SEND);
	closesocket(ClientSocket);
	WSACleanup();
	connected_devices--;
	ids.clear();
	cerr << "------------REBOOTING SOCK------------\n";
	if (!done) return server_init();
	return 0;
}

void read_config(json& config) {	// FIXME: if config file is edited by hand, all this breaks if anything has a trailing comma
	if (config[config.begin().key()].contains("profiles")) {
		ids[0].profiles.clear();
		int profile_count = 0;
		for (auto& profile : config[config.begin().key()]["profiles"]) if (profile.is_object()) profile_count++;
		for (int i = 0; i < profile_count; i++) {
			profile profile1;
			if (config[config.begin().key()]["profiles"][to_string(i)].contains("columns")) profile1.columns = std::stoi(config[config.begin().key()]["profiles"][to_string(i)]["columns"].get<string>());
			if (config[config.begin().key()]["profiles"][to_string(i)].contains("rows"))    profile1.rows = std::stoi(config[config.begin().key()]["profiles"][to_string(i)]["rows"].get<string>());
			if (config[config.begin().key()]["profiles"][to_string(i)].contains("pages")) {
				int page_count = 0;
				for (auto& page : config[config.begin().key()]["profiles"][to_string(i)]["pages"]) if (page.is_object()) page_count++;
				for (int j = 0; j < page_count; j++) {
					if (config[config.begin().key()]["profiles"][to_string(i)]["pages"][to_string(i)].contains("buttons")) {
						for (int k = 0; k < profile1.columns * profile1.rows; k++) {
							button button1;
							if (config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"].contains(to_string(k))) {
								if (config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)].contains("label")) button1.label = config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)]["label"];
								button1.label_backup = button1.label;
								if (config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)].contains("has default label")) {
									if (config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)]["has default label"] == "1") button1.default_label = true;
									else if (config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)]["has default label"] == "0") button1.default_label = false;
									else if ((button1.label == "")) button1.default_label = true;
									else button1.default_label = false;
								}
								if (config[config.begin().key()]["profiles"][to_string(i)]["pages"][to_string(j)]["buttons"][to_string(k)].contains("type")) {
									if (config[config.begin().key()]["profiles"][to_string(i)]["pages"][to_string(j)]["buttons"][to_string(k)]["type"] == "0") button1.type = button::types::File;
									else if (config[config.begin().key()]["profiles"][to_string(i)]["pages"][to_string(j)]["buttons"][to_string(k)]["type"] == "1") button1.type = button::types::URL;
									else if (config[config.begin().key()]["profiles"][to_string(i)]["pages"][to_string(j)]["buttons"][to_string(k)]["type"] == "2") button1.type = button::types::Command;
									else button1.type = button::types::File; // TODO: (not urgent) add button to problematic value vector and show a modal popup on attempt.
								}
								if (config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)].contains("action")) button1.action = config[config.begin().key()]["profiles"][to_string(i)]["pages"]["0"]["buttons"][to_string(k)]["action"];
							}
							profile1.buttons.push_back(button1);
						}
					}
				}
			}
			ids[0].profiles.push_back(profile1);
		}
	}
}


void configure_id(id* id) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell\\";
	nxsh_config += id->ID;
	nxsh_config += ".json";
	cerr << nxsh_config << "\n";
	ifstream reader(nxsh_config);
	json config;
	if (reader && reader.peek() != ifstream::traits_type::eof()) {
		config = json::parse(reader);
		cerr << config << "\n";
		read_config(config);
		send_config = true;
		cerr << "configured\n";
	} else {
		profile profile1;
		for (int i = 1; i <= id->profiles[0].columns * id->profiles[0].rows; i++) {
			button button1;
			profile1.buttons.push_back(button1);
		}
		id->profiles.push_back(profile1);
	}
}

void parse_message(string message) {
	if (message.length() == ID_LENGTH) {
		try {
			unsigned long long convert_id = std::stoull(message);
			stringstream convert_back;
			string check_id;
			convert_back << convert_id;
			convert_back >> check_id;
			id ID(message);
			cerr << "ID: " << ID.ID << "\n";
			if ((check_id.length() == ID_LENGTH) &&
				!(std::find(ids.begin(), ids.end(), ID.ID) != ids.end())) {
				ids.push_back(ID);
				configure_id(&ids[0]);
			}
		} catch (...) {}
		return;
	}
	if (message.substr(0, 2) == SHORTCUT_PREFIX) {
		string act_pos = message.substr(2, message.length());
		int pos = std::stoi(act_pos);
		pos--;
		if (!ids[0].profiles[0].buttons[pos].action.empty()) {// TODO: Change to actual ID index after implementing multiple connections (int id_belongs_to_thread = thread_no;) same with profiles
			string nospace_action = "";
			if (ids[0].profiles[0].buttons[pos].type == button::types::URL) nospace_action = "start ";
			else nospace_action += "\"";
			nospace_action += ids[0].profiles[0].buttons[pos].action;
			if (ids[0].profiles[0].buttons[pos].type != button::types::URL) nospace_action += "\"";
			cerr << "Opening: " << nospace_action.c_str() << "\n";
#ifdef _DEBUG
			thread runner([nospace_action]() {
				system(nospace_action.c_str());
			});	
			runner.detach();
#else
			std::wstring wstring_action = std::wstring(nospace_action.begin(), nospace_action.end());
			wchar_t* wchar_action = (wchar_t*)wstring_action.c_str();
			ShellExecute(NULL, L"open", wchar_action, NULL, NULL, SW_SHOW);
							 // "explore" for folders
							 // "properties" for properties
#endif
		}
		return;
	}
	// TODO: add set_screen() function to change "working directory"
	return;
}

int setup_sock(int& iResult,
			   WSADATA& wsaData,
			   SOCKET& ListenSocket,
			   SOCKET& ClientSocket,
			   struct addrinfo hints,
			   struct addrinfo* result) {
	if (iResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) return 1;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	iResult = getaddrinfo(NULL, "27015", &hints, &result);
	if (iResult) {
		WSACleanup();
		return 1;
	}
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		ShowWindow(FindWindow(NULL, L"NexusShell"), SW_NORMAL);
		SetFocus(FindWindow(NULL, L"NexusShell"));
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

void clear_button(int profile, int page, int button) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell";
	if (!exists(nxsh_config)) {
		create_directory(nxsh_config);
		return;
	}
	nxsh_config += "\\";
	nxsh_config += ids[selected_id].ID;
	nxsh_config += ".json";
	json to_remove;
	ifstream reader(nxsh_config);
	if (reader && reader.peek() != ifstream::traits_type::eof()) to_remove = json::parse(reader);
	reader.close();
	to_remove[ids[selected_id].ID]["profiles"][to_string(profile)]["pages"][to_string(page)]["buttons"].erase(to_string(button));
	ofstream writer(nxsh_config);
	writer << to_remove.dump(4);
	writer.close();
	read_config(to_remove);
	button_cleared = true;
	send_config = true;
}

void write_config(vector<string> args, int arg_size) {
	string nxsh_config(getenv("USERPROFILE"));
	nxsh_config += "\\AppData\\Roaming\\NexusShell";
	if (!exists(nxsh_config)) create_directory(nxsh_config);
	nxsh_config += "\\";
	nxsh_config += args[0];
	nxsh_config += ".json";
	json to_write;
	ifstream reader(nxsh_config);
	if (reader && reader.peek() != ifstream::traits_type::eof())  to_write = json::parse(reader);
	reader.close();
	json* current = &to_write;

	for (size_t i = 0; i < arg_size - 1; ++i) {
		if (current->find(args[i]) == current->end()) {
			(*current)[args[i]] = json::object();
		}
		current = &(*current)[args[i]];
	}
	*current = args.back();

	ofstream writer(nxsh_config);
	writer << to_write.dump(4);
	writer.close();
}
