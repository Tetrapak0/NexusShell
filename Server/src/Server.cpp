#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/GUI.h"
#include "../include/Server.h"

int connected_devices = 0;
int config_send_stage = 0;

int server_init() {	// make these global
	sockinfo sock = sock_init();
	if (sock.iResult != -1) begin_accept_cycle(sock);
	else {
		return -1; // todo: Show error message if window not found; add bool bound_sock
	}
	closesocket(sock.ClientSocket);
	WSACleanup();
	connected_devices = 0;
	ids.clear();
	return 0;
}
// TODO: Remove socks map and use ID member variables instead. less to clear and keep track of
sockinfo sock_init() {
	sockinfo si;
	ZeroMemory(&si.hints, sizeof(si.hints));
	if (setup_sock(si)) si.iResult = -1;
	si.ClientSocket = INVALID_SOCKET;
	return si;
}

void begin_accept_cycle(sockinfo& sock) {
	char* idbuf = nullptr;
	bool is_valid;
	vector<std::shared_ptr<thread>> socks;
	sock.ClientSocket = INVALID_SOCKET;
	try {
		idbuf = new char[1024 * 256];
	} catch (std::bad_alloc) {
		cerr << "Failed to allocate initial buffer.\n";
		goto failed_alloc;
	}
	if (idbuf == nullptr) {
		cerr << "Failed to allocate initial buffer.\n";
		goto failed_alloc;
	}
	while (!done) {
		sockinfo accept_sock = sock;
		is_valid = accept_socket(accept_sock, idbuf, is_valid);
		if (!is_valid) { closesocket(accept_sock.ClientSocket); continue; }
		id ID(idbuf);
		cerr << "ID: " << ID.ID << "\n";
		ID.sock = accept_sock;
		configure_id(ID);
		id_map.insert({ID.ID, ID});
		id_map[ID.ID].sock = accept_sock;
		ids.push_back(ID.ID);
		socks.push_back(std::make_unique<thread>(comm));
	}
failed_alloc:
	for (auto& sock : socks) sock->join();
	delete[] idbuf;
}
bool id::operator==(const id& other) const {
	return ID == other.ID;
}
vector<id> ids;

void comm() {
	string ID = ids[connected_devices++];
	char* buffer = nullptr;
	try {
		buffer = new char[1024 * 256];
	} catch (std::bad_alloc) {
		cerr << "Failed to allocate memory for ID: " << ID << "\n";
		goto failed_alloc;
	}
	if (buffer == nullptr) {
		cerr << "Failed to allocate memory for ID: " << ID << "\n";
		goto failed_alloc;
	}
	do {
		id_map[ID].sock.iResult = recv(id_map[ID].sock.ClientSocket, buffer, 19, 0);
		if (id_map[ID].sock.iResult > 0) {
			string message(buffer);
			while (id_map[ID].locked) {}
			id_map[ID].locked = true;
			parse_message(message, id_map[ID]);
			id_map[ID].locked = false;
		}
	} while (!done && id_map[ID].sock.iSendResult > 0 && id_map[ID].sock.iResult > 0);
	delete[] buffer;
failed_alloc:
	closesocket(id_map[ID].sock.ClientSocket);
	id_map.erase(ID);
	vector<string> swapper;
	int ids_size = ids.size();
	int selected_id_back = selected_id;
	int id_index = -1;
	bool incremented = false;
	while (ids_locked) {}
	ids_locked = true;
	selected_id = -1;
	for (vector<string>::iterator it = ids.begin(); it != ids.end(); ++it) {
		if (!incremented) ++id_index;
		if (*it == ID) incremented = true;
		else swapper.push_back(*it); // Not using vector::erase (or that with std::remove), because it moshes data between objects
	}
	cerr << "ID: " << ID << " has disconnected.\n";
	ids.clear();
	ids = swapper;
	if (selected_id_back != id_index && selected_id_back > id_index) {
		selected_id = selected_id_back;
	} else if (!selected_id_back && id_index) {
		selected_id = selected_id_back;
		should_draw_button_properties = false;
		should_draw_id_properties = false;
		clear_dialog_shown = false;
		button_properties_to_draw = -1;
	} else {
		selected_id_id = "";
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
			string check_id = to_string(convert_id);
			if ((check_id.length() == ID_LENGTH))
				for (string& temp_id : ids) if (temp_id == check_id) return false;
				return true;
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
