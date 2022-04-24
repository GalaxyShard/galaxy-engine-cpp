#include <gldebug.hpp>
#if OS_WEB
    #include <emscripten.h>
#endif
void initialize();

#if USE_GLFM
GLFMDisplay *glfmDisplay;

void glfmMain(GLFMDisplay *display)
{
    glfmDisplay = display;
    glfmSetMultitouchEnabled(display, true);
    glfmSetDisplayConfig(display,
        GLFMRenderingAPIOpenGLES2,
        GLFMColorFormatRGBA8888,
        GLFMDepthFormat16,
        GLFMStencilFormatNone,
        GLFMMultisampleNone
    );
    glfmSetSurfaceCreatedFunc(display, [](GLFMDisplay*, int, int) { initialize(); });
}
#endif
#if USE_GLFW
int main()
{
    initialize();
    return 0;
}
#endif