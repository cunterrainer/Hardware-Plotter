#pragma once
#ifdef WINDOWS
    #include "SerialWin32.h"
#elif defined(LINUX)
    #include "SerialLinux.h"
#elif defined(MAC_OS)
    #include "SerialMacOS.h"
#endif