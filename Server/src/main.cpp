#include "../include/Header.h"
#include "../include/GUI.h"

#ifdef _DEBUG
int main(int argc, char** argv) {
#else
INT WINAPI wWinMain(_In_	 HINSTANCE hInstance,
				    _In_opt_ HINSTANCE hPrevInstance,	
				    _In_	 PWSTR	   pCmdLine,
				    _In_	 INT	   nCmdShow) {
#endif
	thread gui_handler(gui_init);
	server_init();
	gui_handler.join();
	return 0;
}
