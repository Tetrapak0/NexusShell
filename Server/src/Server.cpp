#include "../include/Header.h"
#include "../include/Config.h"

int connected_devices = 0;
int config_send_stage = 0;

bool send_clear = false;
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
	//ioctlsocket(ListenSocket, FIONBIO, &mode);
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
				configure_id(&ID);
				ids.push_back(ID);
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
#ifdef _DEBUG
			if (ids[0].profiles[0].buttons[pos].type == button::types::URL) nospace_action = "start ";
			if (ids[0].profiles[0].buttons[pos].type != button::types::URL) nospace_action += "\"";
			nospace_action += ids[0].profiles[0].buttons[pos].action;
			if (ids[0].profiles[0].buttons[pos].type != button::types::URL) nospace_action += "\"";
#else
			nospace_action += "\"";
			nospace_action += ids[0].profiles[0].buttons[pos].action;
			nospace_action += "\"";
#endif
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
