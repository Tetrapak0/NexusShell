#include "../include/Header.h"
#include "../include/Config.h"
#include "../include/Server.h"

int connected_devices = 0;
int comm_threads_active = 0;
int conf_threads_active = 0;

int server_init() {	// make these global
	sockinfo commsock = sock_init(0);
	sockinfo confsock = sock_init(1);
	if (commsock.iResult != -1 && confsock.iResult != -1) {
		begin_accept_cycle(commsock, confsock);
	} else {
		return -1; // todo: Show error message if window not found; add bool bound_sock
	}
	commsock.iResult = shutdown(commsock.ClientSocket, SD_SEND);
	confsock.iResult = shutdown(confsock.ClientSocket, SD_SEND);
	closesocket(commsock.ClientSocket);
	closesocket(confsock.ClientSocket);
	WSACleanup();
	connected_devices--;
	ids.clear();
	return 0;
}

sockinfo sock_init(int type) {
	sockinfo si;
	ZeroMemory(&si.hints, sizeof(si.hints));
	if (setup_sock(si.iResult, si.wsaData, si.ListenSocket, si.ClientSocket, si.hints, si.result, type))
		si.iResult = -1;

	si.ClientSocket = INVALID_SOCKET;
	return si;
}

void begin_accept_cycle(sockinfo& commsock, sockinfo& confsock) {
	char idbuf[19];
	char comm_id[19];
	char conf_id[19];
	bool comm_valid;
	bool conf_valid;
	do {
		commsock.ClientSocket = INVALID_SOCKET;
		confsock.ClientSocket = INVALID_SOCKET;
		conf_valid = accept_socket(confsock, idbuf, conf_id, conf_valid);
		comm_valid = accept_socket(commsock, idbuf, comm_id, comm_valid);
		if (!comm_valid || !conf_valid) continue;
		if (strncmp(comm_id, conf_id, 19) == 0) {
			id ID(comm_id);
			cerr << "ID: " << ID.ID << "\n";
			configure_id(&ID);
			ids.push_back(ID);
			sockinfo communicator_commsock = commsock;
			sockinfo configurator_confsock = confsock;
			thread communicator(comm_comm, communicator_commsock);
			thread configurator(conf_comm, configurator_confsock);
			communicator.detach();
			configurator.detach();
			connected_devices++;
		} // else continue; // This isn't required rn
	} while (!done);
}

int accept_socket(sockinfo& sock, char* idbuf, char* sock_id, bool& id_valid) {
	while (sock.ClientSocket == INVALID_SOCKET) sock.ClientSocket = accept(sock.ListenSocket, NULL, NULL);
	sock.iResult = recv(sock.ClientSocket, idbuf, 19, 0);
	if (is_id(idbuf)) {
		strncpy(sock_id, idbuf, 19);
		return true;
	} else return false;
}

void comm_comm(sockinfo commsock) {
	int comm_thread_no = comm_threads_active++;
	cerr << comm_thread_no << "\n";
	id& comm_id = ids[comm_thread_no];
	comm_id.belongs_to_comm_thread_no = comm_thread_no;	// Is this needed? using id comparison is more reliable
	comm_id.has_commsock			  = true;
	char buffer[19];
	do {
		commsock.iResult = recv(commsock.ClientSocket, buffer, 19, 0);
		if (commsock.iResult > 0) {
			parse_message(buffer);
			commsock.iSendResult = send(commsock.ClientSocket, buffer, commsock.iResult, 0);
		}
	} while (!done && commsock.iSendResult > 0 && commsock.iResult > 0 && comm_id.has_confsock);
	for (int i = 0; i < ids.size(); i++)  if (ids[i].ID == comm_id.ID)  ids.erase(ids.begin() + i);
	comm_threads_active--;
	connected_devices--;
}

void reconfigure(id& id, sockinfo& confsock) {
	do {
		if (id.reconfigure) {
			string nxsh_config(getenv("USERPROFILE"));
			nxsh_config += "\\AppData\\Roaming\\NexusShell\\";
			nxsh_config += id.ID;
			nxsh_config += ".json";
			if (exists(nxsh_config)) {
				ifstream reader(nxsh_config);
				json config = json::parse(reader);
				string configuration = "cfg" + config.dump();
				cerr << configuration << "\n";
				confsock.iSendResult = send(confsock.ClientSocket, configuration.c_str(), configuration.length(), 0);
			}
		}
		id.reconfigure = false;
		Sleep(1000);
	} while (!done && id.has_commsock && id.has_confsock && confsock.iSendResult && confsock.iResult);
}

void conf_comm(sockinfo confsock) {
	int conf_thread_no = ++conf_threads_active - 1;
	cerr << conf_thread_no;
	id& conf_id = ids[conf_thread_no];
	conf_id.belongs_to_conf_thread_no = conf_thread_no;
	conf_id.has_confsock			  = true;

	auto reconfigure_lambda = [&conf_id, &confsock] {
		reconfigure(conf_id, confsock);
	};

	thread configurator(reconfigure_lambda);
	char* buffer = (char*)malloc(sizeof(char) * (1024 * 256));
	do {
		confsock.iResult = recv(confsock.ClientSocket, buffer, 1024 * 256, 0);
	} while (!done && confsock.iSendResult > 0 && confsock.iResult > 0 && conf_id.has_commsock);
	free(buffer);
	conf_threads_active--;
	configurator.join();
}

bool is_id(string message) {
	if (message.length() == ID_LENGTH) {
		try {
			unsigned long long convert_id = std::stoull(message);
			stringstream convert_back;
			string check_id;
			convert_back << convert_id;
			convert_back >> check_id;
			if ((check_id.length() == ID_LENGTH) &&
				!(std::find(ids.begin(), ids.end(), check_id) != ids.end())) {
				return true;
			}
		} catch (...) {}
	}
	return false;
}

void parse_message(string message) {
	if (message.substr(0, 2) == SHORTCUT_PREFIX) {
		string act_pos = message.substr(2, message.length());
		int pos = std::stoi(act_pos);
		pos--;
		if (!ids[0].profiles[0].buttons[pos].action.empty()) {
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
			   struct addrinfo* result,
			   int socktype) {	// 0 for commsock, 1 for confsock, and so on if needed
	if (iResult = WSAStartup(MAKEWORD(2, 2), &wsaData)) return 1;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	if (socktype == 0) iResult = getaddrinfo(NULL, "27015", &hints, &result);	// TODO: Let the user specify the port they wish to use
	else if (socktype == 1) iResult = getaddrinfo(NULL, "27016", &hints, &result);
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
