//
// Created by Karim on 2024-11-18.
//

#include "core/os_core.c"

#if _WIN32
#include "core/win32/os_core_win32.c"
#elif  __linux__
#include "os/core/win32/os_core_linux.c"
#else
#error OS core layer not implemented for this operating system.
#endif
