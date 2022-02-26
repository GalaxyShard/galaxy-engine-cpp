#pragma once
#include <Galaxy/OS/defines.hpp>
#if USE_GLFM
    #include <glfm.h>
    extern GLFMDisplay *glfmDisplay;
#endif
#if USE_GLFW
    #include <GLFW/glfw3.h>
#endif
#ifdef DEBUG
#include <stdio.h>
#include <unordered_map>
extern const std::unordered_map<int, const char*> glErrorStringMap;

static inline bool gl_check_errors()
{
    bool errored = 0;
    while (int e = glGetError())
    {
        errored = 1;
        printf("GL error: %s\n", (glErrorStringMap.count(e) ? glErrorStringMap.at(e) : ""));
    }
    return !errored;
}
    #define GLCall(v) v; if(!gl_check_errors())exit(-1);
#else
    #define GLCall(v) v;
#endif