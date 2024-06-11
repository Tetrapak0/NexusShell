#include "../include/Header.hpp"
#include "../include/Config.hpp"
#include "../include/GUI.hpp"
#include "../include/Server.hpp"

int connections = 0;
int port        = 27015;

bool restart_socket = false;

socket_t listen_socket = NX_INVALID_SOCKET;

int server_init() {
    socket_t listen_socket;
    int res = socket_init(listen_socket);
    if (res == -1) return -1;
    else server_loop(listen_socket);
    nx_sock_close(listen_socket);
    cleanup();
    connections = 0;
    shells_lock.lock();
    shells.clear();
    shells_lock.unlock();
    if (restart_socket) {
        LOG("--------------REBOOTING SOCKET--------------");
        return server_init();
    }
    restart_socket = true;
    done = true;
    return 0;
}

int socket_init(socket_t& listen_socket) {
    // TODO: move to rw_portstore
    string portstore = string(getenv("APPDATA")) + "\\NexusShell\\";
    if (!exists(portstore)) create_directories(portstore);
    portstore += "portstore";
    if (exists(portstore)) {
        ifstream reader(portstore);
        char temp[6];
        reader.getline(temp, 6);
        reader.close();
        string tempstr(temp);
        try {
            port = stoi(tempstr);
        } catch (...) {port = 27015;}
        if (port > 65535) {
            fprintf(stderr, "Port number exceeds 65535 (2^16-1). Defaulting to 27015.\n");
            error_dialog(1, "Port number exceeds 65535 (2^16-1). Defaulting to 27015.\n");
            port = 27015;
            ofstream writer(portstore);
            writer << port;
            writer.close();
        }
    }
    return socket_setup(listen_socket);
}

void server_loop(socket_t listen_socket) {
    restart_socket = false;
    char* idbuf = nullptr;
    bool valid;
    vector<std::shared_ptr<thread>> socks;
    socket_t client_socket = NX_INVALID_SOCKET;
    try {
        idbuf = new char[1024 * 256];
    } catch (std::bad_alloc) {
        LOG("Failed to allocate memory for verification buffer.");
        error_dialog(1, "Failed to allocate memory for 1verification buffer.\n");
        exit(-1);
    }
    if (!idbuf) {
        LOG("Failed to allocate memory for verification buffer.");
        error_dialog(1, "Failed to allocate memory for verification buffer.\n");
        exit(-1);
    }
    while (!done && !restart_socket) {
        memset(idbuf, 0, 1024 * 256);
        valid = socket_accept(client_socket, listen_socket, idbuf);
        if (!valid) {
            nx_sock_close(client_socket);
            continue;
        }
        Shell shell;
        shell.ID = idbuf;
        LOGVAR("ID", shell.ID);
        shell.client_socket = client_socket;
        if (configure_shell(shell)) continue;
        shells.insert(std::make_pair(shell.ID, &shell));
        socks.push_back(std::make_unique<thread>(shell_comm_loop, shell.ID));
    }
    for (auto& socket : socks) socket->join();
    delete[] idbuf;
}

int socket_accept(socket_t& client_socket, socket_t listen_socket, char* idbuf) {
    while (client_socket == INVALID_SOCKET && !done && !restart_socket) 
        client_socket = accept(listen_socket, NULL, NULL);
    int res = recv(client_socket, idbuf, 19, 0);
    if (res < ID_LENGTH || idbuf == NULL) return false;
    if (shell_validate(idbuf)) return true;
    return false;
}

void shell_comm_loop(string ID) {
    char* buffer = nullptr;
    Shell& shell = shells[ID];
    int res = 0, send_res = 0;
    try {
        buffer = new char[1024 * 256];
    } catch (std::bad_alloc) {
        LOGVAR("Failed to allocate memory for communication buffer for ID", ID);
        error_dialog(1, ("Failed to allocate memory for communication buffer for ID: " + ID + "\n"));
        goto failed_alloc;
    }
    if (!buffer) {
        LOGVAR("Failed to allocate memory for communication buffer for ID", ID);
        error_dialog(1, ("Failed to allocate memory for communication buffer for ID: " + ID + "\n"));
        goto failed_alloc;
    }
    do {
        res = recv(shell.client_socket, buffer, 19, 0);
        if (res > 0) {
            string message(buffer);
            shell.lock.lock();
            shell_parse_message(message, shell);
            shell.lock.unlock();
        }
    } while (!done && !restart_socket && send_res > 0 && res > 0);
    delete[] buffer;
failed_alloc:
    nx_sock_close(shell.client_socket);
    shells_lock.lock();
    if (selected_shell == ID) {
        clear_dialog_shown = false;
        should_draw_button_properties = false;
        should_draw_shell_properties = false;
        button_properties_to_draw = -1;
        selected_shell = "";
    }
    LOG((shell.nickname + " disconnected.\n"));
    for (size_t i = 0; i < shell.profiles.size(); ++i)
        for (int j = 0; j < shell.profiles[i].size; ++j)
            if (shell.profiles[i].buttons[j].display_type == Button::display_types::Image 
                && shell.profiles[i].buttons[j].thumbnail)
                    SDL_DestroyTexture(shell.profiles[i].buttons[j].thumbnail);
    shells.erase(ID);
    shells_lock.unlock();
}

bool shell_validate(string msg) {
    if (msg.length() == ID_LENGTH) {
        try {
            uint64_t converter = stoull(msg);
            // TODO: Maybe bigint
            string checker = to_string(converter);
            if ((checker.length() == ID_LENGTH))
                for (unordered_map<string, Shell>::iterator it = shells.begin(); 
                     it != shells.end(); ++it) 
                    if (it->first == checker) return false;
            return true;
        } catch (...) {}
    }
    return false;
}

void shell_parse_message(string& msg, Shell& shell) {
    if (msg.substr(0, 2) == SHORTCUT_PREFIX) {
        string act_pos = msg.substr(2, msg.length());
        int pos = stoi(act_pos);    // URGENT: try catch
        if (!CURRENT_PROFILE_SELF.buttons[pos].action.empty()) {
            string cmd = "\"" + CURRENT_PROFILE_SELF.buttons[pos].action +"\"";
            LOGVAR("Opening", cmd);

            switch (shell.profiles[0].buttons[pos].type) {
                case Button::types::Directory:
                    ShellExecuteA(NULL, "explore", cmd.c_str(), NULL, NULL, SW_SHOW);
                    break;
                default: ShellExecuteA(NULL, "open", cmd.c_str(), NULL, NULL, SW_SHOW);
            }
        } // TODO: POSIX execution
    }
}

int socket_setup(socket_t& listen_socket) {
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    int res = 0;
    #ifdef _WIN32
        WSADATA wsadata;
        res = WSAStartup(MAKEWORD(2, 2), &wsadata);
        if (res) {
            LPSTR err_msg;
            DWORD fmt_res = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                          NULL, res, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&err_msg, 0, NULL);
            fprintf(stderr, "WSAStartup failed: %s\n", err_msg);
            LocalFree(err_msg);
            return res;
        }
    #endif

    listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == NX_INVALID_SOCKET) {
        cleanup();
        return -1;
    }

    res = bind(listen_socket, (sockaddr*)&addr, sizeof(addr));
    if (res == NX_SOCKET_ERROR) {
        #ifdef _WIN32
            HANDLE nxsh_window = FindWindowA(NULL, "NexusShell");
            if (nxsh_window != INVALID_HANDLE_VALUE) {
                ShowWindow((HWND)nxsh_window, SW_NORMAL);
                SetFocus((HWND)nxsh_window);
            } else error_dialog(1, ("Port " + to_string(port) +
                                    " already occupied by another program.\n" +
                                    "Edit configuration file to use another port."));
            nx_sock_close(listen_socket);
        #endif
        cleanup();
        return -1;
    }
    res = listen(listen_socket, SOMAXCONN);
    if (res == NX_SOCKET_ERROR) {
        nx_sock_close(listen_socket);
        cleanup();
        return -1;
    }
    return 0;
}
