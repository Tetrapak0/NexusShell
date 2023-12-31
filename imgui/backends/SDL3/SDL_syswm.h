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

/**
 *  \file SDL_syswm.h
 *
 *  \brief Include file for SDL custom system window manager hooks.
 */

#ifndef SDL_syswm_h_
#define SDL_syswm_h_

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_platform_defines.h"
#include "SDL_video.h"
 
/**
 *  \brief SDL_syswm.h
 *
 *  Your application has access to a special type of event ::SDL_EVENT_SYSWM,
 *  which contains window-manager specific information and arrives whenever
 *  an unhandled window event occurs.  This event is ignored by default, but
 *  you can enable it with SDL_SetEventEnabled().
 */

/**
 *  The available subsystems based on platform
 */
#ifndef SDL_DISABLE_SYSWM_PLATFORMS

#ifndef SDL_DISABLE_SYSWM_COCOA
#ifdef __MACOS__
#define SDL_ENABLE_SYSWM_COCOA
#endif
#endif /* !SDL_DISABLE_SYSWM_COCOA */

#ifndef SDL_DISABLE_SYSWM_WAYLAND
#if defined(__LINUX__) || defined(__FREEBSD__)
#define SDL_ENABLE_SYSWM_WAYLAND
#endif
#endif /* !SDL_DISABLE_SYSWM_WAYLAND */

#ifndef SDL_DISABLE_SYSWM_WINDOWS
#if defined(__WIN32__) || defined(__GDK__)
#define SDL_ENABLE_SYSWM_WINDOWS
#endif
#endif /* !SDL_DISABLE_SYSWM_WINDOWS */

#ifndef SDL_DISABLE_SYSWM_X11
#if defined(__unix__) && !defined(__WIN32__)
#define SDL_ENABLE_SYSWM_X11
#endif
#endif /* !SDL_DISABLE_SYSWM_X11 */

#endif /* !SDL_DISABLE_SYSWM_PLATFORMS */

/**
 *  Forward declaration of types used by subsystems
 */
#ifndef SDL_DISABLE_SYSWM_TYPES

#if defined(SDL_ENABLE_SYSWM_COCOA) && !defined(SDL_DISABLE_SYSWM_COCOA_TYPES)
#ifdef __OBJC__
@class NSWindow;
#else
typedef struct _NSWindow NSWindow;
#endif
#endif /* SDL_ENABLE_SYSWM_COCOA */

#if defined(SDL_ENABLE_SYSWM_WAYLAND) && !defined(SDL_DISABLE_SYSWM_WAYLAND_TYPES)
struct wl_display;
struct wl_egl_window;
struct wl_surface;
struct xdg_popup;
struct xdg_positioner;
struct xdg_surface;
struct xdg_toplevel;
#endif /* SDL_ENABLE_SYSWM_WAYLAND */

#if defined(SDL_ENABLE_SYSWM_WINDOWS) && !defined(SDL_DISABLE_SYSWM_WINDOWS_TYPES)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX   /* don't define min() and max(). */
#define NOMINMAX
#endif
#include <windows.h>
#endif /* SDL_ENABLE_SYSWM_WINDOWS */

#if defined(SDL_ENABLE_SYSWM_X11) && !defined(SDL_DISABLE_SYSWM_X11_TYPES)
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif /* SDL_ENABLE_SYSWM_X11 */

#endif /* !SDL_DISABLE_SYSWM_TYPES */

#include "SDL_begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* This is the current version of structures in this file */
#define SDL_SYSWM_CURRENT_VERSION   1
#define SDL_SYSWM_INFO_SIZE_V1      (16 * (sizeof (void *) >= 8 ? sizeof (void *) : sizeof(Uint64)))
#define SDL_SYSWM_CURRENT_INFO_SIZE SDL_SYSWM_INFO_SIZE_V1

/* This is the tag associated with a Metal view so you can find it */
#define SDL_METALVIEW_TAG 255


/**
 *  These are the various supported windowing subsystems
 */
typedef enum
{
    SDL_SYSWM_UNKNOWN,
    SDL_SYSWM_ANDROID,
    SDL_SYSWM_COCOA,
    SDL_SYSWM_HAIKU,
    SDL_SYSWM_KMSDRM,
    SDL_SYSWM_RISCOS,
    SDL_SYSWM_UIKIT,
    SDL_SYSWM_VIVANTE,
    SDL_SYSWM_WAYLAND,
    SDL_SYSWM_WINDOWS,
    SDL_SYSWM_WINRT,
    SDL_SYSWM_X11
} SDL_SYSWM_TYPE;

/**
 *  The custom event structure.
 */
struct SDL_SysWMmsg
{
    Uint32 version;
    Uint32 subsystem;                   /**< SDL_SYSWM_TYPE */

    Uint32 padding[(2 * (sizeof (void *) >= 8 ? sizeof (void *) : sizeof(Uint64)) - 2 * sizeof(Uint32)) / sizeof(Uint32)];

    union
    {
#ifdef SDL_ENABLE_SYSWM_WINDOWS
        struct {
            HWND hwnd;                  /**< The window for the message */
            UINT msg;                   /**< The type of message */
            WPARAM wParam;              /**< WORD message parameter */
            LPARAM lParam;              /**< LONG message parameter */
        } win;
#endif
#ifdef SDL_ENABLE_SYSWM_X11
        struct {
            XEvent event;
        } x11;
#endif
        /* Can't have an empty union */
        int dummy;
    } msg;
};

/**
 *  The custom window manager information structure.
 *
 *  When this structure is returned, it holds information about which
 *  low level system it is using, and will be one of SDL_SYSWM_TYPE.
 */
struct SDL_SysWMinfo
{
    Uint32 version;
    Uint32 subsystem;                   /**< SDL_SYSWM_TYPE */

    Uint32 padding[(2 * (sizeof (void *) >= 8 ? sizeof (void *) : sizeof(Uint64)) - 2 * sizeof(Uint32)) / sizeof(Uint32)];

    union
    {
#ifdef SDL_ENABLE_SYSWM_WINDOWS
        struct
        {
            HWND window;                /**< The window handle */
            HDC hdc;                    /**< The window device context */
            HINSTANCE hinstance;        /**< The instance handle */
        } win;
#endif
#ifdef SDL_ENABLE_SYSWM_X11
        struct
        {
            Display *display;           /**< The X11 display */
            int screen;                 /**< The X11 screen */
            Window window;              /**< The X11 window */
        } x11;
#endif
#ifdef SDL_ENABLE_SYSWM_COCOA
        struct
        {
#if defined(__OBJC__) && defined(__has_feature)
        #if __has_feature(objc_arc)
            NSWindow __unsafe_unretained *window; /**< The Cocoa window */
        #else
            NSWindow *window;                     /**< The Cocoa window */
        #endif
#else
            NSWindow *window;                     /**< The Cocoa window */
#endif
        } cocoa;
#endif
#ifdef SDL_ENABLE_SYSWM_WAYLAND
        struct
        {
            struct wl_display *display;             /**< Wayland display */
            struct wl_surface *surface;             /**< Wayland surface */
            struct wl_egl_window *egl_window;       /**< Wayland EGL window (native window) */
            struct xdg_surface *xdg_surface;        /**< Wayland xdg surface (window manager handle) */
            struct xdg_toplevel *xdg_toplevel;      /**< Wayland xdg toplevel role */
            struct xdg_popup *xdg_popup;            /**< Wayland xdg popup role */
            struct xdg_positioner *xdg_positioner;  /**< Wayland xdg positioner, for popup */
        } wl;
#endif

        /* Make sure this union has enough room for 14 pointers */
        void *dummy_ptrs[14];
        Uint64 dummy_ints[14];
    } info;
};
SDL_COMPILE_TIME_ASSERT(SDL_SysWMinfo_size, sizeof(struct SDL_SysWMinfo) == SDL_SYSWM_CURRENT_INFO_SIZE);

typedef struct SDL_SysWMinfo SDL_SysWMinfo;


/**
 * Get driver-specific information about a window.
 *
 * You must include SDL_syswm.h for the declaration of SDL_SysWMinfo.
 *
 * \param window the window about which information is being requested
 * \param info an SDL_SysWMinfo structure filled in with window information
 * \param version the version of info being requested, should be
 *                SDL_SYSWM_CURRENT_VERSION
 * \returns 0 on success or a negative error code on failure; call
 *          SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC int SDLCALL SDL_GetWindowWMInfo(SDL_Window *window, SDL_SysWMinfo *info, Uint32 version);


/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "SDL_close_code.h"

#endif /* SDL_syswm_h_ */
