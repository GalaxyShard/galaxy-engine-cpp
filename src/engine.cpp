#include <Galaxy/init.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Input/input.hpp>
#include <iostream>

#include <Math/internaltime.hpp>
#include <gldebug.hpp>
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

#if USE_GLFM
#define SWAP_BUFFERS() glfmSwapBuffers(glfmDisplay);
#endif
#if USE_GLFW
static GLFWwindow *window;
#define SWAP_BUFFERS() glfwSwapBuffers(window);
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
    glfwSetWindowRefreshCallback(window, [](GLFWwindow* window)
    {
        int w, h;
        glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
        InternalTime::start_frame();
        Renderer::fix_aspect(w, h);
        Renderer::draw_all(1);
        InternalTime::end_frame();
        glfwSwapBuffers(window);
    });
    glfwSetWindowSizeLimits(window, 600, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);
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
void initialize()
{
    std::cout.setf(std::ios::fixed);
    srand(time(0)); // use a combination of time and something else for seed, eg mac address
    rand();
#if USE_GLFM
    glfmSetSurfaceResizedFunc(glfmDisplay, [](GLFMDisplay*, int w, int h)
    { Renderer::fix_aspect(w, h); });
#endif
#if USE_GLFW
    if (!init_glfw()) return;
#endif
    printf("GL %s\n", glGetString(GL_VERSION));
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
#if USE_GLFM
    glfmSetRenderFunc(glfmDisplay, [](GLFMDisplay*)
    {
        static bool didInit = 0;
        if (!didInit) Init::fire();
        didInit = 1;
        
        redraw();
    });
    glfmSetSurfaceDestroyedFunc(glfmDisplay, [](GLFMDisplay*)
    {
        Init::fire_cleanup();
        printf("Cleanup fired\n");
    });
#endif
#if USE_GLFW
    Init::fire();
    while (!glfwWindowShouldClose(window))
    {
        redraw();
    }
    Init::fire_cleanup();
    glfwTerminate();
#endif
}