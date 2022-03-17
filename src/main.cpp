#include <gldebug.hpp>
#ifdef DEBUG
#include <unordered_map>
const std::unordered_map<unsigned int, const char*> glErrorStringMap =
{
    {GL_INVALID_ENUM, "Invalid enum"},
    {GL_INVALID_VALUE, "Invalid value"},
    {GL_INVALID_OPERATION, "Invalid operation"},
    {GL_OUT_OF_MEMORY, "Out of memory"},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "Invalid framebuffer operation"},
};
#endif
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