//
// Created by Abdik on 2024-11-18.
//

#ifndef OS_INC_H
#define OS_INC_H

#include "core/os_core.h"

#if _WIN32
# include "core/win32/os_core_win32.h"
#elif defined(__linux__)
# include "core/linux/os_core_linux.h"
#else
# error OS core layer not implemented for this operating system.
#endif

#endif //OS_INC_H
