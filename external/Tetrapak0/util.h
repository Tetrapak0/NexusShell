#ifndef __UTIL_H__
#define __UTIL_H__

#ifdef _WIN32
#include <windows.h>
#include <winuser.h>
#include <shellapi.h>
#endif

void warn(const char* format, ...);
void error(const char* format, ...);

int swap_endian(int num);

#endif
