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

#ifndef SDL_main_h_
#define SDL_main_h_

#include "SDL_stdinc.h"

/**
 *  \file SDL_main.h
 *
 *  \brief Redefine main() on some platforms so that it is called by SDL.
 */

#ifndef SDL_MAIN_HANDLED
#ifdef __WIN32__
/* On Windows SDL provides WinMain(), which parses the command line and passes
   the arguments to your main function.

   If you provide your own WinMain(), you may define SDL_MAIN_HANDLED
 */
#define SDL_MAIN_AVAILABLE

#endif
#endif /* SDL_MAIN_HANDLED */

#ifndef SDLMAIN_DECLSPEC
#define SDLMAIN_DECLSPEC
#endif

/**
 *  \file SDL_main.h
 *
 *  The application's main() function must be called with C linkage,
 *  and should be declared like this:
 *  \code
 *  #ifdef __cplusplus
 *  extern "C"
 *  #endif
 *  int main(int argc, char *argv[])
 *  {
 *  }
 *  \endcode
 */

#if defined(SDL_MAIN_NEEDED) || defined(SDL_MAIN_AVAILABLE)
#define main    SDL_main
#endif

#include "SDL_begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The prototype for the application's main() function
 */
typedef int (*SDL_main_func)(int argc, char *argv[]);
extern SDLMAIN_DECLSPEC int SDL_main(int argc, char *argv[]);


/**
 * Circumvent failure of SDL_Init() when not using SDL_main() as an entry
 * point.
 *
 * This function is defined in SDL_main.h, along with the preprocessor rule to
 * redefine main() as SDL_main(). Thus to ensure that your main() function
 * will not be changed it is necessary to define SDL_MAIN_HANDLED before
 * including SDL.h.
 *
 * \since This function is available since SDL 3.0.0.
 *
 * \sa SDL_Init
 */
extern DECLSPEC void SDLCALL SDL_SetMainReady(void);

/**
 * Initializes and launches an SDL application, by doing platform-specific
 * initialization before calling your mainFunction and cleanups after it
 * returns, if that is needed for a specific platform, otherwise it just calls
 * mainFunction.
 *
 * You can use this if you want to use your own main() implementation without
 * using SDL_main (like when using SDL_MAIN_HANDLED). When using this, you do
 * *not* need SDL_SetMainReady().
 *
 * \param argc The argc parameter from the application's main() function, or 0
 *             if the platform's main-equivalent has no argc
 * \param argv The argv parameter from the application's main() function, or
 *             NULL if the platform's main-equivalent has no argv
 * \param mainFunction Your SDL app's C-style main(), an SDL_main_func. NOT
 *                     the function you're calling this from! Its name doesn't
 *                     matter, but its signature must be like int my_main(int
 *                     argc, char* argv[])
 * \param reserved should be NULL (reserved for future use, will probably be
 *                 platform-specific then)
 * \returns the return value from mainFunction: 0 on success, -1 on failure;
 *          SDL_GetError() might have more information on the failure
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC int SDLCALL SDL_RunApp(int argc, char* argv[], SDL_main_func mainFunction, void * reserved);

#ifdef __WIN32__

/**
 * Register a win32 window class for SDL's use.
 *
 * This can be called to set the application window class at startup. It is
 * safe to call this multiple times, as long as every call is eventually
 * paired with a call to SDL_UnregisterApp, but a second registration attempt
 * while a previous registration is still active will be ignored, other than
 * to increment a counter.
 *
 * Most applications do not need to, and should not, call this directly; SDL
 * will call it when initializing the video subsystem.
 *
 * \param name the window class name, in UTF-8 encoding. If NULL, SDL
 *             currently uses "SDL_app" but this isn't guaranteed.
 * \param style the value to use in WNDCLASSEX::style. If `name` is NULL, SDL
 *              currently uses `(CS_BYTEALIGNCLIENT | CS_OWNDC)` regardless of
 *              what is specified here.
 * \param hInst the HINSTANCE to use in WNDCLASSEX::hInstance. If zero, SDL
 *              will use `GetModuleHandle(NULL)` instead.
 * \returns 0 on success or a negative error code on failure; call
 *          SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC int SDLCALL SDL_RegisterApp(const char *name, Uint32 style, void *hInst);

/**
 * Deregister the win32 window class from an SDL_RegisterApp call.
 *
 * This can be called to undo the effects of SDL_RegisterApp.
 *
 * Most applications do not need to, and should not, call this directly; SDL
 * will call it when deinitializing the video subsystem.
 *
 * It is safe to call this multiple times, as long as every call is eventually
 * paired with a prior call to SDL_RegisterApp. The window class will only be
 * deregistered when the registration counter in SDL_RegisterApp decrements to
 * zero through calls to this function.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_UnregisterApp(void);

#endif /* defined(__WIN32__) */

#ifdef __cplusplus
}
#endif

#include "SDL_close_code.h"

#if !defined(SDL_MAIN_HANDLED) && !defined(SDL_MAIN_NOIMPL)
/* include header-only SDL_main implementations */
#ifdef __WIN32__

/* platforms which main (-equivalent) can be implemented in plain C */
#include "SDL_main_impl.h"

#endif /* __WIN32__ */

#endif /* SDL_MAIN_HANDLED */

#endif /* SDL_main_h_ */
