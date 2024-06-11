#include "../include/Header.hpp"
#include "../include/GUI.hpp"
#include "../include/Server.hpp"

typedef struct nxsh_flags_s {
    int gflags = GF_NONE;
} nxsh_flags;

static nxsh_flags parse_args(int argc, const char*** const argv) {
    nxsh_flags out;
    for (int i = 0; i < argc; ++i) {
        if (!strcmp((*argv)[i], "-n") || !strcmp((*argv)[i], "--nowindow"))
            out.gflags |= GF_HIDDEN;
        else if (!strcmp((*argv)[i], "-s") || !strcmp((*argv)[i], "--softrenderer"))
            out.gflags |= GF_SOFTWARE;
    }
    return out;
}

#ifdef _WIN32
int _main(int argc, char** argv) {
#else
int main(int argc, char** argv) {
#endif
    string nxsh_path = NX_PATH_GET();
    nxsh_flags flags = parse_args(argc, (const char*** const)&argv);
    thread tray_handler(systray_init, 0);
    thread server_handler(server_init);
    gui_init(flags.gflags);

    return 0; // TODO: Use return bit flags from each function to determine success
}

#ifdef _WIN32
    #define main _main
//    #define _DEBUG
    #ifdef _DEBUG
        #include <stdio.h>
        #define _CRTDBG_MAP_ALLOC
        #include <crtdbg.h>
        #include <consoleapi.h>
    #endif
INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     PSTR pCmdLine, INT nCmdShow) {
    #ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    AllocConsole();
    AttachConsole(GetCurrentProcessId());

    freopen("CON", "w", stdout);
    freopen("CON", "w", stderr);
    freopen("CON", "r", stdin);

    printf("Console initialized successfully!\n"
           "NexusShell ver: %s. _DEBUG is defined.\n", NXSH_VERSION);
    #endif
    int ret = main(__argc, __argv);
    #ifdef _DEBUG
    FreeConsole();
    #endif
    return ret;
}
#endif
