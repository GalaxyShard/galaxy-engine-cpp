
#include <Galaxy/Input/input.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/print.hpp>
#include <iostream>

#include <internalinit.hpp>
#include <unordered_map>
#include <gldebug.hpp>
#if OS_WEB
    #include <emscripten/html5.h>
#endif

template class ArgCallback<bool>;

struct InputData
{
    const char *bind;
    KeyCode key;
    input_callback callback;

    InputData(const char *bind, KeyCode key, input_callback callback)
        : bind(bind), key(key), callback(callback) { }
};
extern UIImage **heldImages;
extern int lastTouchID;
namespace
{
    Vector2 mousePos = Vector2(-1,-1);
    auto onTouchEvent = EventT<TouchData>();
    auto onKeyEvent = EventT<KeyData>();

    struct CompareCStr
    {
        // std::map requires true when the a < b
        bool operator()(const char *a, const char *b) const
        { return strcmp(a, b)<0; }
    };
    auto lastTouchState = std::unordered_map<int, TouchData>();
    auto callbacks = std::unordered_multimap<KeyCode, InputData>();
    auto bindMap = std::map<const char*, KeyCode, CompareCStr>();
    auto lastBindState = std::map<const char*, bool, CompareCStr>();
    
    void process_key(KeyCode key, bool isPressed, int mods)
    {
        (void)mods;
        onKeyEvent.fire({key, isPressed});

        auto iterator = callbacks.equal_range(key);
        for (auto i = iterator.first; i != iterator.second; ++i)
        {
            bool &state = lastBindState[i->second.bind];
            if (state != isPressed)
            {
                state = isPressed;
                i->second.callback(isPressed);
            }
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
        (void)mods;
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
        // mousePos is converted, -1 is bottom left of inner square, pos is [-1, 1]
        // will be updated when outside of window as long as window is focused
        mousePos = Vector2(x / Renderer::screenWidth, 1.f-(y / Renderer::screenHeight))*2.f-1.f;
        mousePos *= Renderer::aspectRatio;
    }
#if OS_WEB
    int em_key_callback(int eventType, const EmscriptenKeyboardEvent *e, void*)
    {
        if (e->repeat)
            return 0;
        static std::map<const char*, KeyCode, CompareCStr> webCodeToKey = {
            {"ControlLeft",Key::LeftControl},
            {"ControlRight",Key::RightControl},

            {"AltLeft",Key::LeftAlt},
            {"AltRight",Key::RightAlt},

            {"ShiftLeft",Key::LeftShift},
            {"ShiftRight",Key::RightShift},

            {"BracketLeft",Key::LeftBracket},
            {"BracketRight",Key::RightBracket},

            {"ArrowLeft",Key::Left},
            {"ArrowRight",Key::Right},
            {"ArrowUp",Key::Up},
            {"ArrowDown",Key::Down},

            {"Space",Key::Space},
            {"Backquote",Key::GraveAccent},
            {"Minus",Key::Minus},
            {"Equal",Key::Equal},
            {"Tab",Key::Tab},
            {"Backslash",Key::Backslash},
            {"Semicolon",Key::Semicolon},
            {"Quote",Key::Apostrophe},
            {"Comma",Key::Comma},
            {"Period",Key::Period},
            {"Slash",Key::Slash},
            {"Enter",Key::Enter},
            {"Escape",Key::Escape},
            {"Backspace",Key::Backspace},
            {"CapsLock",Key::CapsLock}
        };
        
        KeyCode key;

        // e->code is mod-insensitive, e->key is
        if (strncmp(e->code, "Key", 3)==0) // pressing any letter will result in Key#, where # is the letter
            key = (KeyCode)e->code[3];
        else if (strncmp(e->code, "Digit", 5)==0) // pressing any digit will result in Digit#
            key = (KeyCode)e->code[5];
        else if (e->code[0]=='F') // function keys
        {
            if (e->code[2])
                key = (e->code[1]-Key::D0)*10 + (e->code[2]-Key::D0);
            else key = e->code[1]-Key::D0;
            key += Key::F1-1;
        }
        else if (strncmp(e->code, "Meta", 4)==0) // ignore system keys (l/r cmd on osx, win on windows, search on chromeos)
            return 0;
        else
        {
            auto it = webCodeToKey.find(e->code);
            if (it == webCodeToKey.end())
            {
                logmsg("key not recognized: \'%o\'\n", (char*)e->code);
                return 0;
            }
            key = it->second;
        }
        process_key(key, eventType==EMSCRIPTEN_EVENT_KEYDOWN, 0);
        return 1;
    }
#endif
#if USE_GLFM
    bool touch_callback(GLFMDisplay*, int touch, GLFMTouchPhase phase, double x, double y)
    {
        process_cursor(x, y);
        if (phase == GLFMTouchPhaseBegan)
            process_click(touch, 1, 0, Input::mousePos());
        else if (phase == GLFMTouchPhaseEnded)
            process_click(touch, 0, 0, Input::mousePos());
        
        TouchState state;
        if (phase==GLFMTouchPhaseBegan) state = TouchState::PRESSED;
        else if (phase==GLFMTouchPhaseCancelled || phase==GLFMTouchPhaseEnded) state = TouchState::RELEASED;
        else if (phase==GLFMTouchPhaseMoved) state = TouchState::MOVED;
        else return 1;
        
        TouchData data = TouchData();
        data.id = touch;
        data.pos = Input::mousePos();
        data.state = state;
        if (lastTouchState.count(touch))
        {
            data.delta = data.pos - lastTouchState[touch].pos;
            data.startPos = lastTouchState[touch].startPos;
        }
        else data.startPos = data.pos;
        if (state == TouchState::RELEASED) lastTouchState.erase(touch);
        else lastTouchState[touch] = data;
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
        (void)mods, (void)scancode;
        if (action != GLFW_PRESS) return;
        process_key_rebind((KeyCode)key);
        glfwSetKeyCallback(glfwGetCurrentContext(), &key_callback);
    }
    bool isMouseHeld = 0;
    void mouse_callback(GLFWwindow*, int button, int action, int mods)
    {
        (void)button;
        if (action == GLFW_REPEAT) return;
        isMouseHeld = action;
        process_click(0, action, mods, Input::mousePos());
        TouchData data;
        data.id = 0;
        data.pos = Input::mousePos();
        if (!action)
            data.startPos = lastTouchState[data.id].startPos;
        else
            data.startPos = Input::mousePos();
        data.state = action ? PRESSED : RELEASED;

        lastTouchState[data.id] = data;
        onTouchEvent.fire(data);
    }
    void cursor_callback(GLFWwindow*, double x, double y)
    {
        process_cursor(x, y);
        if (!isMouseHeld) return;
        TouchData data;
        data.id = 0;
        data.pos = Input::mousePos();
        data.delta = Input::mousePos() - lastTouchState[data.id].pos;
        data.startPos = lastTouchState[data.id].startPos;
        data.state = MOVED;
        
        lastTouchState[data.id] = data;
        onTouchEvent.fire(data);
    }
#endif
}
Vector2 Input::mousePos() { return ::mousePos; }
void Input::add_bind(const char *bind, KeyCode key)
{
    add_bind(bind, key, nullptr);
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
    return lastBindState[bind];
}
void Input::trigger(const char *bind, bool isPressed)
{
    if (lastBindState[bind] == isPressed)
        return;
    lastBindState[bind] = isPressed;
    KeyCode key = bindMap[bind];

    auto iterator = callbacks.equal_range(key);
    for (auto i = iterator.first; i != iterator.second; ++i)
    {
        if (i->second.bind == bind)
        {
            i->second.callback(isPressed);
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
            return;
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
SignalT<TouchData>& Input::touch_changed() { return onTouchEvent.signal; }
SignalT<KeyData>& Input::key_pressed() { return onKeyEvent.signal; }

void iinit_input()
{
#if OS_WEB
    // Overrides glfm callbacks, but that doesnt matter since they are not used
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, 1, &em_key_callback);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, 1, &em_key_callback);
#endif
#if USE_GLFM
    glfmSetTouchFunc(glfmDisplay, &touch_callback);
#endif
#if USE_GLFW
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