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
 *  \file SDL_system.h
 *
 *  \brief Include file for platform specific SDL API functions
 */

#ifndef SDL_system_h_
#define SDL_system_h_

#include "SDL_stdinc.h"
#include "SDL_keyboard.h"
#include "SDL_render.h"
#include "SDL_video.h"

#include "SDL_begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Platform specific functions for Windows */
#if defined(__WIN32__)

typedef void (SDLCALL * SDL_WindowsMessageHook)(void *userdata, void *hWnd, unsigned int message, Uint64 wParam, Sint64 lParam);

/**
 * Set a callback for every Windows message, run before TranslateMessage().
 *
 * \param callback The SDL_WindowsMessageHook function to call.
 * \param userdata a pointer to pass to every iteration of `callback`
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_SetWindowsMessageHook(SDL_WindowsMessageHook callback, void *userdata);

#endif /* defined(__WIN32__) */

#if defined(__WIN32__)

/**
 * Get the D3D9 adapter index that matches the specified display.
 *
 * The returned adapter index can be passed to `IDirect3D9::CreateDevice` and
 * controls on which monitor a full screen application will appear.
 *
 * \param displayID the instance of the display to query
 * \returns the D3D9 adapter index on success or a negative error code on
 *          failure; call SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC int SDLCALL SDL_Direct3D9GetAdapterIndex(SDL_DisplayID displayID);

typedef struct IDirect3DDevice9 IDirect3DDevice9;

/**
 * Get the D3D9 device associated with a renderer.
 *
 * Once you are done using the device, you should release it to avoid a
 * resource leak.
 *
 * \param renderer the renderer from which to get the associated D3D device
 * \returns the D3D9 device associated with given renderer or NULL if it is
 *          not a D3D9 renderer; call SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC IDirect3DDevice9* SDLCALL SDL_GetRenderD3D9Device(SDL_Renderer * renderer);

typedef struct ID3D11Device ID3D11Device;

/**
 * Get the D3D11 device associated with a renderer.
 *
 * Once you are done using the device, you should release it to avoid a
 * resource leak.
 *
 * \param renderer the renderer from which to get the associated D3D11 device
 * \returns the D3D11 device associated with given renderer or NULL if it is
 *          not a D3D11 renderer; call SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC ID3D11Device* SDLCALL SDL_GetRenderD3D11Device(SDL_Renderer * renderer);

#endif /* defined(__WIN32__) */

#if defined(__WIN32__)

typedef struct ID3D12Device ID3D12Device;

/**
 * Get the D3D12 device associated with a renderer.
 *
 * Once you are done using the device, you should release it to avoid a
 * resource leak.
 *
 * \param renderer the renderer from which to get the associated D3D12 device
 * \returns the D3D12 device associated with given renderer or NULL if it is
 *          not a D3D12 renderer; call SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC ID3D12Device* SDLCALL SDL_RenderGetD3D12Device(SDL_Renderer* renderer);

#endif /* defined(__WIN32__) */

#if defined(__WIN32__)

/**
 * Get the DXGI Adapter and Output indices for the specified display.
 *
 * The DXGI Adapter and Output indices can be passed to `EnumAdapters` and
 * `EnumOutputs` respectively to get the objects required to create a DX10 or
 * DX11 device and swap chain.
 *
 * \param displayID the instance of the display to query
 * \param adapterIndex a pointer to be filled in with the adapter index
 * \param outputIndex a pointer to be filled in with the output index
 * \returns SDL_TRUE on success or SDL_FALSE on failure; call SDL_GetError()
 *          for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC SDL_bool SDLCALL SDL_DXGIGetOutputInfo(SDL_DisplayID displayID, int *adapterIndex, int *outputIndex);

#endif /* defined(__WIN32__) */

/* Platform specific functions for Linux */
#ifdef __LINUX__

/**
 * Sets the UNIX nice value for a thread.
 *
 * This uses setpriority() if possible, and RealtimeKit if available.
 *
 * \param threadID the Unix thread ID to change priority of.
 * \param priority The new, Unix-specific, priority value.
 * \returns 0 on success, or -1 on error.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC int SDLCALL SDL_LinuxSetThreadPriority(Sint64 threadID, int priority);

/**
 * Sets the priority (not nice level) and scheduling policy for a thread.
 *
 * This uses setpriority() if possible, and RealtimeKit if available.
 *
 * \param threadID The Unix thread ID to change priority of.
 * \param sdlPriority The new SDL_ThreadPriority value.
 * \param schedPolicy The new scheduling policy (SCHED_FIFO, SCHED_RR,
 *                    SCHED_OTHER, etc...)
 * \returns 0 on success or a negative error code on failure; call
 *          SDL_GetError() for more information.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC int SDLCALL SDL_LinuxSetThreadPriorityAndPolicy(Sint64 threadID, int sdlPriority, int schedPolicy);

#endif /* __LINUX__ */

/**
 * Query if the current device is a tablet.
 *
 * If SDL can't determine this, it will return SDL_FALSE.
 *
 * \returns SDL_TRUE if the device is a tablet, SDL_FALSE otherwise.
 *
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC SDL_bool SDLCALL SDL_IsTablet(void);

/* Functions used by iOS application delegates to notify SDL about state changes */

/*
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_OnApplicationWillTerminate(void);

/*
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_OnApplicationDidReceiveMemoryWarning(void);

/*
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_OnApplicationWillResignActive(void);

/*
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_OnApplicationDidEnterBackground(void);

/*
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_OnApplicationWillEnterForeground(void);

/*
 * \since This function is available since SDL 3.0.0.
 */
extern DECLSPEC void SDLCALL SDL_OnApplicationDidBecomeActive(void);

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "SDL_close_code.h"

#endif /* SDL_system_h_ */
