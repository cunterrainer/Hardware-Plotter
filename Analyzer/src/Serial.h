#pragma once
#ifdef WINDOWS
    #include "SerialWin32.h"
#elif defined(LINUX)
    #include "SerialLinux.h"
#endif