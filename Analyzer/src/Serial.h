#pragma once
#ifdef WINDOWS
    #include "SerialWin32.h"
#elif defined(UNIX_COMPLIANT) || defined(MAC_OS)
    #include "SerialUnix.h"
#elif defined(LINUX)
    #include "SerialLinux.h"
#endif