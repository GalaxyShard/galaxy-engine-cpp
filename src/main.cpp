#include <gldebug.hpp>
void initialize();

#if USE_GLFM
GLFMDisplay *glfmDisplay;
void glfmMain(GLFMDisplay *display)
{
    glfmDisplay = display;
    glfmSetSurfaceCreatedFunc(display, [](GLFMDisplay*, int, int) { initialize(); });
    glfmSetMultitouchEnabled(display, true);
    glfmSetDisplayConfig(display,
        GLFMRenderingAPIOpenGLES2,
        GLFMColorFormatRGBA8888,
        GLFMDepthFormat16,
        GLFMStencilFormatNone,
        GLFMMultisampleNone
    );
}
#endif
#if USE_GLFW
int main()
{
    initialize();
    return 0;
}
#endif