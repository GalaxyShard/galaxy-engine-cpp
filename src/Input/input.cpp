#include <string>
//#include <Galaxy/init.hpp>
#include <internalinit.hpp>
#include <Galaxy/Input/input.hpp>
#include <Galaxy/Renderer/renderer.hpp>
//#include <Galaxy/UI/canvas.hpp>
#include <Galaxy/UI/image.hpp>
#include <unordered_map>

#include <debug.hpp>
#include <iostream>

struct InputData
{
    const char *bind;
    KeyCode key;
    input_callback callback;

    InputData(const char *bind, KeyCode key, input_callback callback)
        : bind(bind), key(key), callback(callback) { }
};

//InputData::InputData(const char *bind, KeyCode key, input_callback callback)
//    : bind(bind), key(key), callback(callback) { }

//extern UIImage *currentHeldImg;
extern std::unique_ptr<UIImage*[]> heldImages;

Vector2 Input::mousePos = Vector2(-1,-1);//, Input::mouseDelta;
auto onTouchEvent = std::make_unique<EventT<TouchData>>();
namespace
{
    //bool is_within(Vector2 a, Vector2 b, Vector2 scale)
    //{
    //    Vector2 radius = scale * 0.5f;
    //    Vector2 bottomLeft = b-radius;
    //    Vector2 topRight = b+radius;

    //    return Math::within(a.x, bottomLeft.x, topRight.x) && Math::within(a.y, bottomLeft.y, topRight.y);
    //}

    auto lastTouchState = std::make_unique<std::unordered_map<int, TouchData>>();
    auto callbacks = std::make_unique<std::unordered_multimap<KeyCode, InputData>>();
    auto bindMap = std::make_unique<std::unordered_map<const char*, KeyCode>>();
    void process_key(KeyCode key, bool action, int mods)
    {
        auto iterator = callbacks->equal_range(key);

        for (auto i = iterator.first; i != iterator.second; ++i)
        {
            if (i->second.callback != nullptr)
                i->second.callback(action);
        }
    }
    const KeyCode exitRebindKey = KeyCode::Escape;
    void (*onRebindFinish)();
    const char *changingBind;

    void process_key_rebind(KeyCode key)
    {
        if (key != exitRebindKey) Input::rebind(changingBind, key);
        if (onRebindFinish) onRebindFinish();
        
        onRebindFinish = nullptr;
    }
    //bool pos_in_image(Vector2 pos, UIImage *img)
    //{
    //    return is_within(pos, img->calc_world_pos(), img->scale*Renderer::reverseAspect);
    //}
    UIImage *image_in_pos(Vector2 pos)
    {
        const auto &raycastables = UIImage::get_raycastables();
        for (int i = raycastables.size()-1; i >= 0; --i)
        {
            UIImage *img = raycastables[i];
            //if (pos_in_image(pos, img))
            if (img->is_within(pos))
                return img;
        }
        return nullptr;
    }
    // update to fix for mobile
    void process_click(int touchID, bool action, int mods, Vector2 pos)
    {
        const auto &raycastables = UIImage::get_raycastables();
        if (action)
        {
            UIImage *img = image_in_pos(pos);
            if (img)
            {
                heldImages[touchID] = img;
                if (img->onTouchDown) img->onTouchDown();
            }
            //for (int i = raycastables.size()-1; i >= 0; --i)
            //{
            //    UIImage *img = raycastables[i];
//
            //    if (is_within(pos, img->calc_world_pos(), img->scale*Renderer::reverseAspect))
            //    {
            //        currentHeldImg = img;
            //        if (img->onTouchDown) img->onTouchDown();
            //        break;
            //    }
            //}
        }
        //else if (currentHeldImg)
        else if (heldImages[touchID])
        {
            //UIImage *&img = currentHeldImg;
            UIImage *&img = heldImages[touchID];
            if (img->onTouchUp) img->onTouchUp();
            
            // check incase image was deleted inside onTouchUp
            if (img && img->onClick)
            {
                //if (is_within(pos, img->calc_world_pos(), img->scale*Renderer::reverseAspect))
                if (img->is_within(pos))
                {
                    img->onClick();
                }
            }
            heldImages[touchID] = nullptr;
            //currentHeldImg = nullptr;
        }
    }
    void process_cursor(float x, float y)
    {
        // 0,0 is top left, pos is in pixels
        // mousePos is converted, -1 is bottom left, pos is -1~1
        // will be updated when outside of window as long as window is focused
        Input::mousePos = Vector2(x / Renderer::screenWidth, 1.f-(y / Renderer::screenHeight))*2-1;
        //Input::mousePos = Input::mousePos*Renderer::reverseAspect;
        // Implement hover events here?
        //printf("%f, %f\n", Input::mousePos.x, Input::mousePos.y);
    }
#if OS_MOBILE
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
        TouchData data = TouchData {
            .id = touch,
            .pos = Input::mousePos,
            .state = state
        };
        if (lastTouchState->count(touch))
        {
            data.delta = data.pos - (*lastTouchState)[touch].pos;
            data.startPos = (*lastTouchState)[touch].startPos;
        }
        else { data.startPos = data.pos; }
        (*lastTouchState)[touch] = data;
        onTouchEvent->fire(data);
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
        //if (button != MouseCode::LEFT) return; // possibly change
        process_click(0, action, mods, Input::mousePos);
        TouchData data = TouchData {
            .id = 0,
            .pos = Input::mousePos,
            .startPos = Input::mousePos,
            .state = action ? PRESSED : RELEASED
        };
        (*lastTouchState)[data.id] = data;
        onTouchEvent->fire(data);
    }
    void cursor_callback(GLFWwindow*, double x, double y)
    {
        process_cursor(x, y);
        if (isMouseHeld) {
            TouchData data = TouchData {
                .id = 0,
                .pos = Input::mousePos,
                .delta = Input::mousePos - (*lastTouchState)[data.id].pos,
                .startPos = (*lastTouchState)[data.id].startPos,
                .state = MOVED
            };
            (*lastTouchState)[data.id] = data;
            onTouchEvent->fire(data);
        }
    }
#endif
}
void Input::add_bind(const char *bind, KeyCode key, input_callback callback)
{
    if (bindMap->count(bind) == 1)
    {
        fprintf(stderr, "WARNING: bind \'%s\' is already used\n", bind);
        return;
    }
    callbacks->insert(std::make_pair(key, InputData(bind, key, callback)));
    bindMap->insert(std::make_pair(bind, key));
}
void Input::remove_bind(const char *bind)
{
    auto it = callbacks->equal_range((*bindMap)[bind]);
    for (auto i = it.first; i != it.second; ++i)
    {
        if (i->second.bind == bind)
        {
            callbacks->erase(i);
            bindMap->erase(bind);
            return;
        }
    }
}
bool Input::is_held(const char *bind)
{
#if OS_MOBILE
    return 0;
#else
    return glfwGetKey(glfwGetCurrentContext(), (*bindMap)[bind]) == GLFW_PRESS;
#endif
}
void Input::rebind(const char *bind, KeyCode newKey)
{
    auto it = callbacks->equal_range((*bindMap)[bind]);
    for (auto i = it.first; i != it.second; ++i)
    {
        if (i->second.bind == bind)
        {
            i->second.key = newKey;
            (*bindMap)[bind] = newKey;
        }
    }
}
void Input::interactive_rebind(const char *bind, void(*onFinish)())
{
    changingBind = bind;
    onRebindFinish = onFinish;

#if OS_MOBILE
    
#else
    glfwSetKeyCallback(glfwGetCurrentContext(), &rebind_callback);
#endif
}
SignalT<TouchData>& Input::touch_changed() { return *onTouchEvent->signal; }
static void init()
{
#if OS_MOBILE
    glfmSetTouchFunc(glfmDisplay, &touch_callback);
#else
    auto *window = glfwGetCurrentContext();
    glfwSetKeyCallback(window, &key_callback);
    glfwSetMouseButtonCallback(window, &mouse_callback);
    glfwSetCursorPosCallback(window, &cursor_callback);
#endif
}
INTERNAL_INIT_FUNC(init);