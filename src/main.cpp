#include <gldebug.hpp>
#if OS_WEB
    #include <emscripten.h>
#endif
void initialize();

#if USE_GLFM
GLFMDisplay *glfmDisplay;

#if OS_WEB
static bool canInit = 0;
extern "C" void readyToInit()
{
    if (canInit) initialize();
    else canInit = 1;
}
#endif

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
#if OS_WEB
    // https://stackoverflow.com/a/54627719
    EM_ASM({
        FS.mkdir('/gamedata');
        FS.mount(IDBFS, {}, '/gamedata');
        FS.syncfs(true, function(err) {
            if (err) console.log('Error syncing file system: ', err);
            ccall('readyToInit', 'void');
        });
    });
    glfmSetSurfaceCreatedFunc(display, [](GLFMDisplay*, int, int) { readyToInit(); });
#else
    glfmSetSurfaceCreatedFunc(display, [](GLFMDisplay*, int, int) { initialize(); });
#endif
}
#endif
#if USE_GLFW
int main()
{
    initialize();
    return 0;
}
#endif