#include <Galaxy/init.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Math/time.hpp>
#include <Galaxy/Input/input.hpp>

#include <string>

#include <iostream>
#include <debug.hpp>
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

#if OS_MOBILE
#define SWAP_BUFFERS() glfmSwapBuffers(glfmDisplay);

#else
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
        Time::update_delta();
        Renderer::fix_aspect(w, h);
        Renderer::draw_all(1);
        glfwSwapBuffers(window);
    });
    glfwSetWindowSizeLimits(window, 600, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);
    return 1;
}
#endif
static void redraw()
{
    Time::update_delta();
    Renderer::draw_all(true);

    SWAP_BUFFERS();
#if !OS_MOBILE
    glfwPollEvents();
#endif
}
void initialize()
{
    srand(time(0)); // use a combination of time and something else for seed, eg mac address
    rand();
#if OS_MOBILE
    glfmSetSurfaceResizedFunc(glfmDisplay, [](GLFMDisplay*, int w, int h)
    { Renderer::fix_aspect(w, h); });
#else
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

#if OS_MOBILE
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
#else
    Init::fire();
    while (!glfwWindowShouldClose(window))
    {
        redraw();
    }
    Init::fire_cleanup();
    glfwTerminate();
#endif
}