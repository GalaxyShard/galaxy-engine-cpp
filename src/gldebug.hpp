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
#include <assert.h>
extern const std::unordered_map<int, const char*> glErrorStringMap;

static inline bool gl_check_errors(int line, const char *fileName)
{
    bool errored = 0;
    while (int e = glGetError())
    {
        errored = 1;
        //printf("GL error: %s\n", (glErrorStringMap.count(e) ? glErrorStringMap.at(e) : ""));
        //printf("(%s:%d) GL error: %s\n", fileName, line, (glErrorStringMap.count(e) ? glErrorStringMap.at(e) : ""));
        //fprintf(stderr, "(%s:%d) GL error: %s\n", fileName, line, (glErrorStringMap.count(e) ? glErrorStringMap.at(e) : ""));
        Debug::logerror("(%o:%o) GL error: %o\n", fileName, line, (glErrorStringMap.count(e) ? glErrorStringMap.at(e) : ""));
    
    }
    return !errored;
}
    //#define GLCall(v) v; if(!gl_check_errors())exit(-1);
    #define GLCall(v) v; assert(gl_check_errors(__LINE__, __FILE__));
#else
    #define GLCall(v) v;
#endif