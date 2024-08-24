#pragma once
#include <Galaxy/OS/defines.hpp>
#if USE_GLFM
    #include <GLFM/glfm.h>
    extern GLFMDisplay *glfmDisplay;
#endif
#if USE_GLFW
    #include <glad-gl.h>
    #include <GLFW/glfw3.h>
#endif
#if defined(DEBUG) && !OS_WEB
#include <unordered_map>
#include <assert.h>

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
    }
    return !errored;
}
#define GLCall(v) v; assert(gl_check_errors(__LINE__, __FILE__))
#else
#define GLCall(v) v
#endif