#pragma once
#include <Galaxy/OS/defines.hpp>
#if OS_MOBILE || OS_WEB
    #include <glfm.h>
    extern GLFMDisplay *glfmDisplay;
#else
    #include <GLFW/glfw3.h>
#endif
#ifdef DEBUG
#include <unordered_map>
extern std::unordered_map<int, const char*> glErrorStringMap;

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
    #define GLCall(v) v; assert(gl_check_errors());
#else
    #define GLCall(v) v;
#endif