#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/GUI.h"
#include "../include/Server.h"

int connected_devices = 0;

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
		if (configure_id(ID)) continue;
		ids.insert({ID.ID, ID});
		socks.push_back(std::make_unique<thread>(comm, ID.ID));
	}
failed_alloc:
	for (auto& sock : socks) sock->join();
	delete[] idbuf;
}

int accept_socket(sockinfo& sock, char* idbuf, bool& id_valid) {
	while (sock.ClientSocket == INVALID_SOCKET && !done) sock.ClientSocket = accept(sock.ListenSocket, NULL, NULL);
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
		cerr << "Failed to allocate memory for ID: " << ID << "\n";
		goto failed_alloc;
	}
	if (buffer == nullptr) {
		cerr << "Failed to allocate memory for ID: " << ID << "\n";
		goto failed_alloc;
	}
	do {
		comm_id.sock.iResult = recv(comm_id.sock.ClientSocket, buffer, 19, 0);
		if (comm_id.sock.iResult > 0) {
			string message(buffer);
			while (comm_id.locked) {}
			comm_id.locked = true;
			parse_message(message, comm_id);
			comm_id.locked = false;
		}
	} while (!done && comm_id.sock.iSendResult > 0 && comm_id.sock.iResult > 0);
	delete[] buffer;
failed_alloc:
	closesocket(comm_id.sock.ClientSocket);
	ids.erase(ID);
	while (ids_locked) {}
	ids_locked = true;
	if (selected_id == ID) {
		clear_dialog_shown = false;
		should_draw_button_properties = false;
		should_draw_id_properties = false;
		button_properties_to_draw = -1;
		selected_id = "";
	}
	ids_locked = false;
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
			cerr << "Opening: " << nospace_action.c_str() << "\n";

			std::wstring wstring_action = std::wstring(nospace_action.begin(), nospace_action.end());
			wchar_t* wchar_action = (wchar_t*)wstring_action.c_str();

			switch (ID.profiles[0].buttons[pos].type) {
				case button::types::Directory:
					ShellExecute(NULL, L"explore", wchar_action, NULL, NULL, SW_SHOW);
					break;
				default:
					ShellExecute(NULL, L"open", wchar_action, NULL, NULL, SW_SHOW);
			}
		}
		return;
	}
	// TODO: add set_screen() function to change "working directory"
}

int setup_sock(sockinfo& si) {
	if (si.iResult = WSAStartup(MAKEWORD(2, 2), &si.wsaData)) return 1;
	si.hints.ai_family = AF_INET;
	si.hints.ai_socktype = SOCK_STREAM;
	si.hints.ai_protocol = IPPROTO_TCP;
	si.hints.ai_flags = AI_PASSIVE;
	si.iResult = getaddrinfo(NULL, "27015", &si.hints, &si.result);	// TODO: Let the user specify the port
	if (si.iResult) { WSACleanup(); return 1; }
	si.ListenSocket = socket(si.result->ai_family, si.result->ai_socktype, si.result->ai_protocol);
	if (si.ListenSocket == INVALID_SOCKET) {
		freeaddrinfo(si.result);
		WSACleanup();
		return 1;
	}
	si.iResult = bind(si.ListenSocket, si.result->ai_addr, (int)si.result->ai_addrlen);
	if (si.iResult == SOCKET_ERROR) {
		ShowWindow(FindWindow(NULL, L"NexusShell"), SW_NORMAL);
		SetFocus(FindWindow(NULL, L"NexusShell"));
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
