#include <debug.hpp>
#ifdef DEBUG
#include <unordered_map>
//#include <OpenGL/gl3.h>
std::unordered_map<int, const char*> glErrorStringMap =
{
    {GL_INVALID_ENUM, "Invalid enum"},
    {GL_INVALID_VALUE, "Invalid value"},
    {GL_INVALID_OPERATION, "Invalid operation"},
    {GL_OUT_OF_MEMORY, "Out of memory"},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "Invalid framebuffer operation"},
};
#endif
void initialize();

#if OS_MOBILE
GLFMDisplay *glfmDisplay;
void glfmMain(GLFMDisplay *display)
{
    glfmDisplay = display;
    //glfmSetMultitouchEnabled(display, 1);
    glfmSetSurfaceCreatedFunc(display, [](GLFMDisplay*, int, int) { initialize(); });
    glfmSetMultitouchEnabled(display, true);
    glfmSetDisplayConfig(display,
        GLFMRenderingAPIOpenGLES2,
        GLFMColorFormatRGBA8888,
        GLFMDepthFormat24,
        GLFMStencilFormatNone,
        GLFMMultisampleNone
    );
}
#else
int main()
{
    initialize();
    return 0;
}
#endif