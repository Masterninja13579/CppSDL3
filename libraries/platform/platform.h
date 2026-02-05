#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__Win32__) || defined(__NT__)
    #define OS_WINDOWS 1
#elif __APPLE__
    #define OS_MAC 1
#elif __linux__
    #define OS_LINUX 1
#else
    #error "Unsupported Operating System"
#endif
