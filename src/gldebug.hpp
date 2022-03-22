#pragma once
#include <Galaxy/OS/defines.hpp>
#if USE_GLFM
    #include <External/GLFM/glfm.h>
    extern GLFMDisplay *glfmDisplay;
#endif
#if USE_GLFW
    #include <External/GLFW/glfw3.h>
#endif
#ifdef DEBUG
#include <unordered_map>
#include <assert.h>
extern const std::unordered_map<unsigned int, const char*> glErrorStringMap;

static inline bool gl_check_errors(int line, const char *fileName)
{
    bool errored = 0;
    while (unsigned int e = glGetError())
    {
        errored = 1;
        const char *errorStr;
        if (e == GL_INVALID_ENUM) errorStr = "GL_INVALID_ENUM";
        else if (e == GL_INVALID_VALUE) errorStr = "GL_INVALID_VALUE";
        else if (e == GL_INVALID_OPERATION) errorStr = "GL_INVALID_OPERATION";
        else if (e == GL_OUT_OF_MEMORY) errorStr = "GL_OUT_OF_MEMORY";
        else if (e == GL_INVALID_FRAMEBUFFER_OPERATION) errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION";
        else errorStr = "Unrecognized";
        logerr("(%o:%o) GL error: %o\n", fileName, line, errorStr);
        //logerr("(%o:%o) GL error: %o\n", fileName, line, (glErrorStringMap.count(e) ? glErrorStringMap.at(e) : ""));
    }
    return !errored;
}
    #define GLCall(v) v; assert(gl_check_errors(__LINE__, __FILE__))
#else
    #define GLCall(v) v
#endif