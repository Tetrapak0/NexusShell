#include "../include/Header.h"
#include "../include/GUI.h"

int main(int argc, char** argv) {
    thread client_handler(client_init);
    gui_init();
    exit(0);
    return 0;
}
