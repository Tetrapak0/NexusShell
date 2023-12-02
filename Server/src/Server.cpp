#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/GUI.h"
#include "../include/Server.h"

int connected_devices = 0;
int port		      = 27015;

bool restart_socket = false;

sockinfo global_sock;

int server_init() {	// make these global
	global_sock = sock_init();
	if (global_sock.iResult == -1) return -1;
	else begin_accept_cycle(global_sock);
	closesocket(global_sock.ListenSocket);
	WSACleanup();
	connected_devices = 0;
	ids.clear();
	if (restart_socket) {
		LOG("--------------REBOOTING SOCK--------------");
		return server_init();
	}
	restart_socket = true;
	done = true;
	return 0;
}

sockinfo sock_init() {
	sockinfo si;
	ZeroMemory(&si.hints, sizeof(si.hints));
	string portstore = getenv("USERPROFILE");				// TODO: move to rw_portstore
	portstore += "\\AppData\\Roaming\\NexusShell\\";
	if (!exists(portstore)) create_directory(portstore);
	portstore += "portstore";
	if (exists(portstore)) {
		ifstream reader(portstore);
		char temp[6];
		reader.getline(temp, 6);
		reader.close();
		string tempstr(temp);
		try {
			port = std::stoi(tempstr);
		} catch (...) { port = 27015; }
		if (port > 65535) {
			port = 27015;
			ofstream writer(portstore);
			writer << port;
			writer.close();
		}
	}
	if (setup_sock(si)) si.iResult = -1;
	si.ClientSocket = INVALID_SOCKET;
	return si;
}

void begin_accept_cycle(sockinfo& sock) {
	restart_socket = false;
	char* idbuf = nullptr;
	bool is_valid;
	vector<std::shared_ptr<thread>> socks;
	sock.ClientSocket = INVALID_SOCKET;
	try {
		idbuf = new char[1024 * 256];
	} catch (std::bad_alloc) {
		LOG("Failed to allocate initial buffer.");
		error_dialog(1, "Failed to allocate initial buffer.\n");
		exit(-1);
	}
	if (idbuf == nullptr) {
		LOG("Failed to allocate initial buffer.");
		error_dialog(1, "Failed to allocate initial buffer.\n");
		exit(-1);
	}
	while (!done && !restart_socket) {
		sockinfo accept_sock = sock;
		is_valid = accept_socket(accept_sock, idbuf, is_valid);
		if (!is_valid) { closesocket(accept_sock.ClientSocket); continue; }
		id ID(idbuf);
		LOGVAR("ID", ID.ID);
		ID.sock = accept_sock;
		if (configure_id(ID)) continue;
		ids.insert({ID.ID, ID});
		socks.push_back(std::make_unique<thread>(comm, ID.ID));
	}
	for (auto& sock : socks) sock->join();
	delete[] idbuf;
}

int accept_socket(sockinfo& sock, char* idbuf, bool& id_valid) {
	while (sock.ClientSocket == INVALID_SOCKET && !done && !restart_socket) 
		sock.ClientSocket = accept(sock.ListenSocket, NULL, NULL);
	sock.iResult = recv(sock.ClientSocket, idbuf, 19, 0);
	if (is_id(idbuf)) return true;
	return false;
}

void comm(string ID) {
	char* buffer = nullptr;
	id& comm_id = ids[ID];
	try {
		buffer = new char[1024 * 256];
	} catch (std::bad_alloc) {
		LOGVAR("Failed to allocate buffer for ID", ID);
		error_dialog(1, ("Failed to allocate buffer for ID: ", ID, "\n"));
		goto failed_alloc;
	}
	if (buffer == nullptr) {
		LOGVAR("Failed to allocate buffer for ID", ID);
		error_dialog(1, ("Failed to allocate buffer for ID: ", ID, "\n"));
		goto failed_alloc;
	}
	do {
		comm_id.sock.iResult = recv(comm_id.sock.ClientSocket, buffer, 19, 0);
		if (comm_id.sock.iResult > 0) {
			string message(buffer);
			while (comm_id.locked.load()) {}
			comm_id.locked.exchange(true);
			parse_message(message, comm_id);
			comm_id.locked.exchange(false);
		}
	} while (!done && !restart_socket && comm_id.sock.iSendResult > 0 && comm_id.sock.iResult > 0);
	delete[] buffer;
failed_alloc:
	closesocket(comm_id.sock.ClientSocket);
	while (ids_locked.load()) {}
	ids_locked.exchange(true);
	if (selected_id == ID) {
		clear_dialog_shown = false;
		should_draw_button_properties = false;
		should_draw_id_properties = false;
		button_properties_to_draw = -1;
		selected_id = "";
	}
	ids.erase(ID);
	ids_locked.exchange(false);
}

bool is_id(string message) {
	if (message.length() == ID_LENGTH) {
		try {
			unsigned long long convert_id = std::stoull(message);
			string check_id = to_string(convert_id);
			if ((check_id.length() == ID_LENGTH))
				for (unordered_map<string, id>::iterator it = ids.begin(); it != ids.end(); ++it) if (it->first == check_id) return false;
			return true;
		} catch (...) {}
	}
	return false;
}

void parse_message(string& message, id& ID) {
	if (message.substr(0, 2) == SHORTCUT_PREFIX) {
		string act_pos = message.substr(2, message.length());
		int pos = std::stoi(act_pos);
		pos--;
		if (!CURRENT_PROFILE_PAR.buttons[pos].action.empty()) {
			string nospace_action = "\"";
			nospace_action += CURRENT_PROFILE_PAR.buttons[pos].action;
			nospace_action += "\"";
			LOGVAR("Opening", nospace_action);

			wstring wstring_action(nospace_action.begin(), nospace_action.end());

			switch (ID.profiles[0].buttons[pos].type) {
				case button::types::Directory:
					ShellExecute(NULL, L"explore", wstring_action.c_str(), NULL, NULL, SW_SHOW);
					break;
				default:
					ShellExecute(NULL, L"open", wstring_action.c_str(), NULL, NULL, SW_SHOW);
			}
		}
		return;
	}
}

int setup_sock(sockinfo& si) {
	if (si.iResult = WSAStartup(MAKEWORD(2, 2), &si.wsaData)) return 1;
	si.hints.ai_family = AF_INET;
	si.hints.ai_socktype = SOCK_STREAM;
	si.hints.ai_protocol = IPPROTO_TCP;
	si.hints.ai_flags = AI_PASSIVE;
	si.iResult = getaddrinfo(NULL, to_string(port).c_str(), &si.hints, &si.result);
	if (si.iResult) { WSACleanup(); return 1; }
	si.ListenSocket = socket(si.result->ai_family, si.result->ai_socktype, si.result->ai_protocol);
	if (si.ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(si.result);
		WSACleanup();
		return 1;
	}
	si.iResult = bind(si.ListenSocket, si.result->ai_addr, (int)si.result->ai_addrlen);
	if (si.iResult == SOCKET_ERROR) {
		HANDLE nxsh_window = FindWindow(NULL, L"NexusShell");
		if (nxsh_window != INVALID_HANDLE_VALUE) {
			ShowWindow(FindWindow(NULL, L"NexusShell"), SW_NORMAL);
			SetFocus(FindWindow(NULL, L"NexusShell"));
		} else
			error_dialog(1, "Port already occupied by another program.\nEdit configuration to use another port.");
		freeaddrinfo(si.result);
		closesocket(si.ListenSocket);
		WSACleanup();
		return 1;
	}
	if (listen(si.ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		closesocket(si.ListenSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}
