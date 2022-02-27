#pragma once

/* OS_WINDOWS OS_PC OS_IOS OS_MOBILE OS_MAC OS_LINUX OS_ANDROID OS_WEB */

#ifdef EMSCRIPTEN
    #define OS_WEB 1
#else

    #ifdef _WIN32
        #define OS_WINDOWS 1
        #define OS_PC 1
    #endif

    #ifdef __APPLE__
        #include <TargetConditionals.h>

        #if TARGET_OS_SIMULATOR || TARGET_OS_IPHONE
            #define OS_IOS 1
            #define OS_MOBILE 1
            
        #elif TARGET_OS_MAC
            #define OS_MAC 1
            #define OS_PC 1
        #endif
    #endif
    #ifdef __linux__
        #define OS_LINUX 1
        #define OS_PC 1
    #endif

    #ifdef __ANDROID__
        #define OS_ANDROID 1
        #define OS_MOBILE 1
    #endif

#endif


#ifndef OS_PC
    #define OS_PC 0
#endif
#ifndef OS_MOBILE
    #define OS_MOBILE 0
#endif
#ifndef OS_WEB
    #define OS_WEB 0
#endif

#ifndef OS_WINDOWS
    #define OS_WINDOWS 0
#endif
#ifndef OS_MAC
    #define OS_MAC 0
#endif
#ifndef OS_LINUX
    #define OS_LINUX 0
#endif

#ifndef OS_IOS
    #define OS_IOS 0
#endif
#ifndef OS_ANDROID
    #define OS_ANDROID 0
#endif

#if OS_MOBILE || OS_WEB
    #define USE_GLFM 1
#endif
#if OS_PC
    #define USE_GLFW 1
#endif