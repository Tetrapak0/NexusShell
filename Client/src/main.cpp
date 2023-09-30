#include "../include/Header.h"
#include "../include/GUI.h"

int main(int, char**)
{
    thread client_handler(client_init);
    gui_init();
    client_handler.join();
    return 0;
}
