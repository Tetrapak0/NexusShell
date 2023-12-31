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
 *  \file SDL_platform_defines.h
 *
 *  \brief Try to get a standard set of platform defines.
 */

#ifndef SDL_platform_defines_h_
#define SDL_platform_defines_h_

#if (defined(linux) || defined(__linux) || defined(__linux__))
#undef __LINUX__
#define __LINUX__   1
#endif

#ifdef __APPLE__
/* lets us know what version of macOS we're compiling on */
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>

/* Fix building with older SDKs that don't define these
   See this for more information:
   https://stackoverflow.com/questions/12132933/preprocessor-macro-for-os-x-targets
*/
#ifndef TARGET_OS_MACCATALYST
#define TARGET_OS_MACCATALYST 0
#endif

#undef __MACOS__
#define __MACOS__  1
#if MAC_OS_X_VERSION_MIN_REQUIRED < 1070
# error SDL for macOS only supports deploying on 10.7 and above.
#endif /* MAC_OS_X_VERSION_MIN_REQUIRED < 1070 */
#endif /* defined(__APPLE__) */

#if defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
/* Try to find out if we're compiling for WinRT, GDK or non-WinRT/GDK */
#if defined(_MSC_VER) && defined(__has_include)
#if __has_include(<winapifamily.h>)
#define HAVE_WINAPIFAMILY_H 1
#else
#define HAVE_WINAPIFAMILY_H 0
#endif

/* If _USING_V110_SDK71_ is defined it means we are using the Windows XP toolset. */
#elif defined(_MSC_VER) && (_MSC_VER >= 1700 && !_USING_V110_SDK71_)    /* _MSC_VER == 1700 for Visual Studio 2012 */
#define HAVE_WINAPIFAMILY_H 1
#else
#define HAVE_WINAPIFAMILY_H 0
#endif

#if HAVE_WINAPIFAMILY_H
#include <winapifamily.h>
#define WINAPI_FAMILY_WINRT (!WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP))
#else
#define WINAPI_FAMILY_WINRT 0
#endif /* HAVE_WINAPIFAMILY_H */

#if HAVE_WINAPIFAMILY_H && HAVE_WINAPIFAMILY_H
#define SDL_WINAPI_FAMILY_PHONE (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#else
#define SDL_WINAPI_FAMILY_PHONE 0
#endif

#if WINAPI_FAMILY_WINRT
#undef __WINRT__
#define __WINRT__ 1
#elif defined(_GAMING_DESKTOP) /* GDK project configuration always defines _GAMING_XXX */
#undef __WINGDK__
#define __WINGDK__ 1
#elif defined(_GAMING_XBOX_XBOXONE)
#undef __XBOXONE__
#define __XBOXONE__ 1
#elif defined(_GAMING_XBOX_SCARLETT)
#undef __XBOXSERIES__
#define __XBOXSERIES__ 1
#else
#undef __WINDOWS__
#define __WINDOWS__ 1
#endif
#endif /* defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) */

#ifdef __WINDOWS__
#undef __WIN32__
#define __WIN32__ 1
#endif

#endif /* SDL_platform_defines_h_ */
