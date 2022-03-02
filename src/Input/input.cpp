
#include <Galaxy/Input/input.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/UI/image.hpp>
#include <iostream>

#include <internalinit.hpp>
#include <unordered_map>
#include <gldebug.hpp>

template class ArgCallback<bool>;

struct InputData
{
    const char *bind;
    KeyCode key;
    input_callback callback;

    InputData(const char *bind, KeyCode key, input_callback callback)
        : bind(bind), key(key), callback(callback) { }
};
extern std::unique_ptr<UIImage*[]> heldImages;
extern int lastTouchID;

Vector2 Input::mousePos = Vector2(-1,-1);
auto onTouchEvent = EventT<TouchData>();
namespace
{
    auto lastTouchState = std::unordered_map<int, TouchData>();
    auto callbacks = std::unordered_multimap<KeyCode, InputData>();
    auto bindMap = std::unordered_map<const char*, KeyCode>();
    auto lastKeyStates = std::unordered_map<KeyCode, bool>();
    
    void process_key(KeyCode key, bool action, int mods)
    {
        if (lastKeyStates[key] == action)
            return;
        
        lastKeyStates[key] = action;
        auto iterator = callbacks.equal_range(key);

        for (auto i = iterator.first; i != iterator.second; ++i)
        {
            i->second.callback(action);
        }
    }
    bool (*onRebindFinish)(KeyCode key);
    const char *changingBind;

    void process_key_rebind(KeyCode key)
    {
        if (onRebindFinish)
        {
            if (onRebindFinish(key))
                Input::rebind(changingBind, key);
            onRebindFinish = nullptr;
        }
        else Input::rebind(changingBind, key);
    }
    UIImage *image_in_pos(Vector2 pos)
    {
        const auto &raycastables = UIImage::get_raycastables();
        for (int i = raycastables.size()-1; i >= 0; --i)
        {
            UIImage *img = raycastables[i];
            if (img->is_within(pos))
                return img;
        }
        return nullptr;
    }
    void process_click(int touchID, bool action, int mods, Vector2 pos)
    {
        lastTouchID = touchID;
        if (action)
        {
            UIImage *img = image_in_pos(pos); 
            if (img)
            {
                heldImages[touchID] = img;
                img->onTouchDown();
            }
        }
        else if (heldImages[touchID])
        {
            UIImage *&img = heldImages[touchID];
            img->onTouchUp();
            
            // check img incase it was deleted inside onTouchUp
            if (img && img->onClick && img->is_within(pos))
                img->onClick();
            heldImages[touchID] = nullptr;
        }
    }
    void process_cursor(float x, float y)
    {
        // 0,0 is top left, pos is in pixels
        // mousePos is converted, -1 is bottom left, pos is -1~1
        // will be updated when outside of window as long as window is focused
        Input::mousePos = Vector2(x / Renderer::screenWidth, 1.f-(y / Renderer::screenHeight))*2-1;
    }
#if USE_GLFM
    bool touch_callback(GLFMDisplay*, int touch, GLFMTouchPhase phase, double x, double y)
    {
        process_cursor(x, y);
        if (phase == GLFMTouchPhaseBegan)
        {
            process_click(touch, 1, 0, Input::mousePos);
        }
        else if (phase == GLFMTouchPhaseEnded)
        {
            process_click(touch, 0, 0, Input::mousePos);
        }
        TouchState state;
        if (phase==GLFMTouchPhaseBegan) state = TouchState::PRESSED;
        else if (phase==GLFMTouchPhaseCancelled || phase==GLFMTouchPhaseEnded) state = TouchState::RELEASED;
        else if (phase==GLFMTouchPhaseMoved) state = TouchState::MOVED;
        else return 1;
        TouchData data = TouchData {
            .id = touch,
            .pos = Input::mousePos,
            .state = state
        };
        if (lastTouchState.count(touch))
        {
            data.delta = data.pos - lastTouchState[touch].pos;
            data.startPos = lastTouchState[touch].startPos;
        }
        else { data.startPos = data.pos; }
        lastTouchState[touch] = data;
        onTouchEvent.fire(data);
        return 1;
    }
#else
    void key_callback(GLFWwindow*, int key, int, int action, int mods)
    {
        if (action == GLFW_REPEAT) return;
        process_key((KeyCode)key, action, mods);
    }
    void rebind_callback(GLFWwindow*, int key, int scancode, int action, int mods)
    {
        if (action != GLFW_PRESS) return;
        process_key_rebind((KeyCode)key);
        glfwSetKeyCallback(glfwGetCurrentContext(), &key_callback);
    }
    bool isMouseHeld = 0;
    void mouse_callback(GLFWwindow*, int button, int action, int mods)
    {
        if (action == GLFW_REPEAT) return;
        isMouseHeld = action;
        process_click(0, action, mods, Input::mousePos);
        TouchData data = TouchData {
            .id = 0,
            .pos = Input::mousePos,
            .startPos = Input::mousePos,
            .state = action ? PRESSED : RELEASED
        };
        lastTouchState[data.id] = data;
        onTouchEvent.fire(data);
    }
    void cursor_callback(GLFWwindow*, double x, double y)
    {
        process_cursor(x, y);
        if (isMouseHeld) {
            TouchData data = TouchData {
                .id = 0,
                .pos = Input::mousePos,
                .delta = Input::mousePos - lastTouchState[data.id].pos,
                .startPos = lastTouchState[data.id].startPos,
                .state = MOVED
            };
            lastTouchState[data.id] = data;
            onTouchEvent.fire(data);
        }
    }
#endif
}

void Input::add_bind(const char *bind, KeyCode key)
{
    add_bind(bind, key, input_callback());
}
void Input::add_bind(const char *bind, KeyCode key, input_callback callback)
{
    if (bindMap.count(bind) == 1)
    {
        logerr("WARNING: bind \'%o\' is already used\n", bind);
        return;
    }
    callbacks.insert(std::make_pair(key, InputData(bind, key, callback)));
    bindMap.insert(std::make_pair(bind, key));
}
void Input::remove_bind(const char *bind)
{
    if (!bindMap.count(bind))
        return;
    auto it = callbacks.equal_range(bindMap[bind]);
    for (auto i = it.first; i != it.second; ++i)
    {
        if (i->second.bind == bind)
        {
            callbacks.erase(i);
            bindMap.erase(bind);
            return;
        }
    }
}
bool Input::is_held(const char *bind)
{
    return lastKeyStates[bindMap[bind]];
}
void Input::trigger(const char *bind, bool action)
{
    KeyCode key = bindMap[bind];
    if (lastKeyStates[key] == action)
        return;
    
    lastKeyStates[key] = action;
    auto iterator = callbacks.equal_range(key);
    for (auto i = iterator.first; i != iterator.second; ++i)
    {
        if (i->second.bind == bind)
        {
            i->second.callback(action);
            return;
        }
    }
}
void Input::rebind(const char *bind, KeyCode newKey)
{
    auto it = callbacks.equal_range(bindMap[bind]);
    for (auto i = it.first; i != it.second; ++i)
    {
        if (i->second.bind == bind)
        {
            i->second.key = newKey;
            bindMap[bind] = newKey;
        }
    }
}
void Input::interactive_rebind(const char *bind, bool(*onFinish)(KeyCode key))
{
    changingBind = bind;
    onRebindFinish = onFinish;

#if USE_GLFW
    glfwSetKeyCallback(glfwGetCurrentContext(), &rebind_callback);
#endif
}
SignalT<TouchData>& Input::touch_changed() { return *onTouchEvent.signal; }

static void init()
{
#if USE_GLFM
    glfmSetTouchFunc(glfmDisplay, &touch_callback);
#else
    auto *window = glfwGetCurrentContext();
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_callback);
    glfwSetCursorPosCallback(window, &cursor_callback);
    glfwSetWindowFocusCallback(window, [](GLFWwindow *window, int)
    {
        double x,y;
        glfwGetCursorPos(window, &x, &y);
        cursor_callback(0, x, y);
    });
#endif
}
INTERNAL_INIT_FUNC(init);