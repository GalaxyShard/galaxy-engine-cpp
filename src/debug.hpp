#pragma once
//#include <OpenGL/gl3.h>
//#include <GLES/gl2.h>
#include <Galaxy/OS/defines.hpp>
#if OS_MOBILE
    #include <glfm.h>
    extern GLFMDisplay *glfmDisplay;
#else
    #include <GLFW/glfw3.h>
#endif
#include <unordered_map>
extern std::unordered_map<int, const char*> glErrorStringMap;

static inline bool gl_check_errors()
{
    bool errored = 0;
    while (int e = glGetError())
    {
        errored = 1;
        printf("GL error: %s\n", glErrorStringMap[e]);
        //std::cout << "GL error: " << glErrorStringMap[e] << std::endl;
    }
    return !errored;
}
#ifdef DEBUG
    #define GLCall(v) v; assert(gl_check_errors());
#else
    #define GLCall(v) v;
#endif