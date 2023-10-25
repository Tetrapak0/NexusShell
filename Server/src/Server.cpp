#include "../include/Header.h"

int connected_devices = 0; // TODO: make all private and pass them into functions from main.cpp

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

	// TODO: create separate thread for each connection
	//		 id[i].connected;
	connected_devices++;
	if (setup_sock(iResult, wsaData, ListenSocket, ClientSocket, hints, result)) {done = true; return 1;}
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	bool restart = false;
	thread pager(ping, restart, iSendResult, iResult, ClientSocket);
	int recvbuflen = 128;
	char recvbuf[128];
	string message;
	do {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			cerr << "Bytes received: " << iResult << "\n";
			message = recvbuf;
			cerr << "Message: " << message << "\n";
			parse_command(message);
			if (message.length() == ID_LENGTH) {
				try {
					unsigned long long convert_id = std::stoull(message);
					stringstream convert_back;
					string check_id;
					convert_back << convert_id;
					convert_back >> check_id;
					id ID(message);
					cerr << ID.ID << "\n";
					if ((check_id.length() == ID_LENGTH) &&
						!(std::find(ids.begin(), ids.end(), ID.ID) != ids.end())) {
						ids.push_back(ID);
					}
				} catch (...) {}
			}
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) break;
			else cerr << "Bytes sent: " << iSendResult << "\n";
		}
	} while (iResult > 0 || iSendResult > 0 || !done);
	iResult = shutdown(ClientSocket, SD_SEND);
	closesocket(ClientSocket);
	WSACleanup();
	connected_devices = 0;
	ids.clear();
	cerr << "------------REBOOTING SOCK------------\n";
	restart = true;
	pager.join();
	if (!done) return server_init();
	return 0;
}

void ping(bool restart, int iSendResult, int iResult, SOCKET ClientSocket) { // FIXME: Doesnt do shit
	do {
		Sleep(5000);
		iSendResult = send(ClientSocket, "ping", iResult, 0);
	} while (!done || !restart);
}

vector<string> actions = { "code", "explorer", "explorer https://github.com/Tetrapak0"};  // WTF: stop hardcoding
void parse_command(string message) {
	if (message.substr(0, 2) == SHORTCUT_PREFIX) {
		string act_pos = message.substr(2, message.length());
		int pos = std::stoi(act_pos);		// Unsafe? yes. Do I care? no, should be fine.
		if (actions.size() >= pos) {
			pos--;
			system(actions[pos].c_str());
		} else {
			cerr << "No action at index " << pos << ". Starting explorer.\n";
			system("explorer");
		}
	}
	// TODO: add set_screen() function to change "working directory"
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
		ShowWindow(FindWindow(NULL, L"ShortPad"), SW_NORMAL);
		SetFocus(FindWindow(NULL, L"ShortPad"));
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
	// TODO: Close socket on accept and create a new server_init thread
	//		 make sure to join them afterwards
	return 0;
}

void write_config(vector<string> args, int arg_size) {
	string shp_config(getenv("USERPROFILE"));
	shp_config += "\\AppData\\Roaming\\ShortPad";
	if (!exists(shp_config)) create_directory(shp_config);
	shp_config += "\\config.json";
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
