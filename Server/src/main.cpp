#include "../include/Header.h"
#include "../include/GUI.h"

int main(int, char**) {
	thread gui_handler(gui_init);
	server_init();
	cerr << "joining\n";
	gui_handler.join();
	return 0;
}
