#include <Galaxy/init.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Input/input.hpp>
#include <iostream>

#include <Math/internaltime.hpp>
#include <gldebug.hpp>
#if OS_WEB
    #include <emscripten/html5.h>
#endif
/*
Rendering APIs

Metal: MSL
OpenGL: GLSL
OpenGL ES: ESSL (aka GLSL ES)
DirectX: HLSL
PS4: PSSL


MacOS: Metal, OpenGL (deprecated)
iOS: Metal, OpenGL ES (deprecated)

Android: OpenGL ES

Windows: DirectX, OpenGL, Vulkan
Xbox: DirectX

PS4: GNM (low level), GNMX (high level)
*/

extern const char *gameName;
struct AutoCleanup
{
    ~AutoCleanup()
    {
        logmsg("Cleaning up engine\n");
        Init::fire_cleanup();
    }
};
static auto autoCleanup = std::make_unique<AutoCleanup>();

#if USE_GLFM
#define SWAP_BUFFERS() glfmSwapBuffers(glfmDisplay)
#endif
#if USE_GLFW
static GLFWwindow *window;
#define SWAP_BUFFERS() glfwSwapBuffers(window)
static bool init_glfw()
{
    glfwSetErrorCallback([](int code, const char *err)
    {
        std::cout << "an error occured: " << code << " " << err;
    });
    if (!glfwInit()) return 0;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // set opengl version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // set opengl version to 3.3
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // disable legacy opengl
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const int windowWidth = 600, windowHeight = 600;
    window = glfwCreateWindow(windowWidth, windowHeight, gameName, nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 0;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowRefreshCallback(window, [](GLFWwindow*)
    {
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        InternalTime::start_frame();
        Renderer::fix_aspect(w, h);
        Renderer::draw_all(1);
        InternalTime::end_frame();
        glfwSwapBuffers(window);
    });
    glfwSetWindowSizeLimits(window, 400, 225, GLFW_DONT_CARE, GLFW_DONT_CARE);
    return 1;
}
#endif
static void redraw()
{
    InternalTime::start_frame();
    Renderer::draw_all(true);
    InternalTime::end_frame();

    SWAP_BUFFERS();
#if USE_GLFW
    glfwPollEvents();
#endif
}
#if OS_WEB
extern "C" EMSCRIPTEN_KEEPALIVE void readyToInit()
{
    static bool didInit = 0;
    if (didInit) return;
    didInit = 1;

    Init::fire();
    glfmSetRenderFunc(glfmDisplay, [](GLFMDisplay*) { redraw(); });
}
void initFS(bool firstTime)
{
    if (firstTime)
    {
        EM_ASM({
            FS.mkdir('/gamedata');
            FS.mount(IDBFS, {}, '/gamedata');
        });
    }
    // https://stackoverflow.com/a/54627719
    EM_ASM({
        FS.syncfs(true, function(err) {
            if (err) console.log('Error syncing file system: ', err);
            ccall('readyToInit', 'void');
        });
    });
    glClearColor(0.25,0.25,0.75,1);
    glClear(GL_COLOR_BUFFER_BIT);
}
#endif
void initialize()
{
    std::cout.setf(std::ios::fixed);
    srand(time(0)); // todo: use a combination of time and something else for seed, eg mac address
    rand();
#if USE_GLFM
    glfmSetSupportedInterfaceOrientation(glfmDisplay, GLFMInterfaceOrientationAllButUpsideDown);
    glfmSetSurfaceResizedFunc(glfmDisplay, [](GLFMDisplay*, int w, int h)
    { Renderer::fix_aspect(w, h); });
#endif
#if USE_GLFW
    if (!init_glfw()) return;
#endif
#if OS_WEB
    {
        bool enabled = emscripten_webgl_enable_extension(
            emscripten_webgl_get_current_context(),
            "OES_element_index_uint"
        );
        assert(enabled && "Failed to enable extension");
    }
#endif
    logmsg("GL %o\n", glGetString(GL_VERSION));
/*
    enables transparency & sets the equation
    first multiplies the new/source color by the source alpha,
    then adds it to the current/destination color multiplied by one minus the source alpha 
*/
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));
    GLCall(glDepthFunc(GL_LEQUAL));

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));
    GLCall(glFrontFace(GL_CCW)); // counter clockwise vertex ordering

    InternalTime::initialize();
#if OS_WEB
    // Required for iOS, which sometimes doesnt run the syncfs callback. Allows the user to rerun syncfs by tapping the screen after 2 second.
    glfmSetRenderFunc(glfmDisplay, [](GLFMDisplay*)
    {
        initFS(1);
        glfmSetRenderFunc(glfmDisplay, [](GLFMDisplay*)
        {
            logmsg("Waiting for IndexedDB...\n");
            static float endTime = Time::get()+2;
            if (Time::get() < endTime)
                return;
            endTime = Math::INF;
            glClearColor(0.5,0.5,0.5,1);
            glClear(GL_COLOR_BUFFER_BIT);
            // this is run before Init::fire(), so touchFunc is not already set
            glfmSetTouchFunc(glfmDisplay, [](GLFMDisplay*,int,GLFMTouchPhase phase,double,double)->bool
            {
                if (phase==GLFMTouchPhaseBegan)
                {
                    glClearColor(0.75,0.75,0.75,1);
                    glClear(GL_COLOR_BUFFER_BIT);
                }
                else if (phase==GLFMTouchPhaseEnded)
                {
                    logmsg("Reloading IndexedDB...\n");
                    glfmSetTouchFunc(glfmDisplay, nullptr);
                    endTime = Time::get()+2;
                    initFS(0);
                }
                return 1;
            });
        });
    });
#elif USE_GLFM
    glfmSetRenderFunc(glfmDisplay, [](GLFMDisplay*)
    {
        Init::fire();
        glfmSetRenderFunc(glfmDisplay, [](GLFMDisplay*) { redraw(); });
    });
#endif
#if USE_GLFW
    Init::fire();
    while (!glfwWindowShouldClose(window))
        redraw();
    glfwTerminate();
#endif
}