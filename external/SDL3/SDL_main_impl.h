/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2023 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SDL_main_impl_h_
#define SDL_main_impl_h_

#ifndef SDL_main_h_
#error "This header should not be included directly, but only via SDL_main.h!"
#endif

/* if someone wants to include SDL_main.h but doesn't want the main handing magic,
   (maybe to call SDL_RegisterApp()) they can #define SDL_MAIN_HANDLED first
   SDL_MAIN_NOIMPL is for SDL-internal usage (only affects implementation,
   not definition of SDL_MAIN_AVAILABLE etc in SDL_main.h) and if the user wants
   to have the SDL_main implementation (from this header) in another source file
   than their main() function, for example if SDL_main requires C++
   and main() is implemented in plain C */
#if !defined(SDL_MAIN_HANDLED) && !defined(SDL_MAIN_NOIMPL)

/* the implementations below must be able to use the implement real main(), nothing renamed
   (the user's main() will be renamed to SDL_main so it can be called from here) */
#ifdef main
#  undef main
#endif /* main */

#ifdef __WIN32__

/* these defines/typedefs are needed for the WinMain() definition */
#ifndef WINAPI
#define WINAPI __stdcall
#endif

#include <SDL3/SDL_begin_code.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HINSTANCE__ * HINSTANCE;
typedef char* LPSTR;
typedef wchar_t* PWSTR;

/* The VC++ compiler needs main/wmain defined, but not for GDK */
#if defined(_MSC_VER) && !defined(__GDK__)

/* This is where execution begins [console apps] */
#if defined( UNICODE ) && UNICODE
int wmain(int argc, wchar_t *wargv[], wchar_t *wenvp)
{
    (void)argc;
    (void)wargv;
    (void)wenvp;
    return SDL_RunApp(0, NULL, SDL_main, NULL);
}
#else /* ANSI */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    return SDL_RunApp(0, NULL, SDL_main, NULL);
}
#endif /* UNICODE */

#endif /* _MSC_VER && ! __GDK__ */

/* This is where execution begins [windowed apps and GDK] */
#if defined( UNICODE ) && UNICODE
int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrev, PWSTR szCmdLine, int sw)
#else /* ANSI */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
#endif
{
    (void)hInst;
    (void)hPrev;
    (void)szCmdLine;
    (void)sw;
    return SDL_RunApp(0, NULL, SDL_main, NULL);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#include <SDL3/SDL_close_code.h>

/* end of __WIN32__ impl */
#else /* platforms that use a standard main() and just call SDL_RunApp(), like iOS and 3DS */

#include "SDL_begin_code.h"

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char *argv[])
{
    return SDL_RunApp(argc, argv, SDL_main, NULL);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#include "SDL_close_code.h"

/* end of impls for standard-conforming platforms */

#endif /* __WIN32__ etc */

/* rename users main() function to SDL_main() so it can be called from the wrappers above */
#define main    SDL_main

#endif /* SDL_MAIN_HANDLED */

#endif /* SDL_main_impl_h_ */
