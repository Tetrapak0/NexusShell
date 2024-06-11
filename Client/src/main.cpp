#include "../include/Header.hpp"
#include "../include/GUI.hpp"
#include "../include/Client.hpp"
#include "../include/Config.hpp"

int main(int argc, char** argv) {
    nxsh_dir = string(getenv("HOME")) + "/.config/NexusShell/";
    create_directories(nxsh_dir);
    thread client_handler(client_init);
    gui_init();
    client_handler.join();
    return 0;
}
