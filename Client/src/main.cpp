#include "../include/Header.h"
#include "../include/GUI.h"

int main(int, char**) {
    thread client_handler(client_init);
    gui_init();
    exit(0); // recv has ~68 years before timeout, so just kill it.
    return 0;
}
