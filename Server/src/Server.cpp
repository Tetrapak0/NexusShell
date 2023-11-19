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
	char idbuf[19];
	bool is_valid;
	vector<std::shared_ptr<thread>> socks;
	sock.ClientSocket = INVALID_SOCKET;
	while (!done) {
		sockinfo accept_sock = sock;
		is_valid = accept_socket(accept_sock, idbuf, is_valid);
		if (!is_valid) { closesocket(accept_sock.ClientSocket); continue; }
		id ID(idbuf);
		cerr << "ID: " << ID.ID << "\n";
		ID.sock = accept_sock;
		configure_id(ID);
		ids.push_back(ID);
		socks.push_back(std::make_unique<thread>(comm, accept_sock));
	}
	for (auto& sock : socks) sock->join();
}

int accept_socket(sockinfo& sock, char* idbuf, bool& id_valid) {
	while (sock.ClientSocket == INVALID_SOCKET && !done) sock.ClientSocket = accept(sock.ListenSocket, NULL, NULL);
	sock.iResult = recv(sock.ClientSocket, idbuf, 19, 0);
	if (is_id(idbuf)) return true;
	return false;
}

void comm(sockinfo sock) {
	string ID = ids[connected_devices++].ID;
	char buffer[19];	// FIXME: Potential buffer overflow, make sure to also fix in accept cycle
	ids[connected_devices - 1].sock = sock;
	do {
		sock.iResult = recv(sock.ClientSocket, buffer, 19, 0);
		if (sock.iResult > 0) {
			string message(buffer);
			for (int i = 0; i < connected_devices; i++) {
				if (ids[i].ID == ID) {
					while (ids[i].locked) {}
					ids[i].locked = true;
					ids[i].sock = sock;
					parse_message(message, ids[i]);
					ids[i].locked = false;
				}
			}
			//sock.iSendResult = send(sock.ClientSocket, buffer, sock.iResult, 0);
		}
	} while (!done && sock.iSendResult > 0 && sock.iResult > 0);
	vector<id> swapper;
	int ids_size = ids.size();
	int selected_id_back = selected_id;
	int id_index = -1;
	bool incremented = false;
	string selected_id_id_back = selected_id_id;
	selected_id = -1;
	selected_id_id = "";
	for (vector<id>::iterator it = ids.begin(); it != ids.end(); ++it) {
		if (!incremented) ++id_index;
		cerr << "ID from iter: " << (*it).ID << "\n";
		cerr << "rows from iter: " << (*it).profiles[0].rows << "\n";
		if ((*it).ID == ID)  incremented = true;
		else swapper.push_back(*it); // Not using vector::erase (or that with std::remove), because it moshes data between objects
	}
	ids.clear();
	ids = swapper;
	if (selected_id_back != id_index && selected_id_back > id_index) {
		selected_id_id_back = ids[--selected_id_back].ID;
		selected_id = selected_id_back;
		selected_id_id = selected_id_id_back;
	} else if (!selected_id_back){
		selected_id = selected_id_back;
		should_draw_properties = false;
		clear_dialog_shown = false;
		properties_to_draw = -1;
		selected_id_id = selected_id_id_back;
	}
	for (vector<id>::iterator it = ids.begin(); it != ids.end(); ++it) {
		cerr << "IDs: " << (*it).ID << "\n";
		cerr << "profile1 rows: " << (*it).profiles[0].rows << "\n";
	}
	closesocket(sock.ClientSocket);
	connected_devices--;
}

bool is_id(string message) {
	if (message.length() == ID_LENGTH) {
		try {
			unsigned long long convert_id = std::stoull(message);
			stringstream convert_back;
			string check_id;
			convert_back << convert_id;
			convert_back >> check_id;
			if ((check_id.length() == ID_LENGTH))
				for (id& temp_id : ids) if (temp_id.ID == check_id) return false;
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
		if (!ID.profiles[0].buttons[pos].action.empty()) {
			string nospace_action = "\"";
			nospace_action += ID.profiles[0].buttons[pos].action;
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
