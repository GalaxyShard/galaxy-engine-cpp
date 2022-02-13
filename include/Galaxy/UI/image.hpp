#pragma once
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Events/event.hpp>
#include <vector>

class Shader;
class Mesh;
class Scene;
class UIGroup;
//template <typename T>
//class ArgCallback;

//enum ImageState { NONE, TOUCH_DOWN, TOUCH_UP, CLICK };

class UIImage
{
private:
    static std::unique_ptr<std::vector<UIImage *>> images;
    
    unsigned int imageID, rendererID;
    int renderOrder = 0;
    Scene *scene;

    static Mesh *mesh();
    Shader *shader();

    friend class Renderer;
    friend class UIObject;
public:
    UIImage(Texture *texture = nullptr);
    ~UIImage();

    Texture *texture = 0;
    Material *material = 0;
    UIGroup *group = 0;
    Vector2 pos, scale = Vector2(1, 1), anchor = Vector2(-1, -1);
    Vector4 tint = Vector4(1, 1, 1, 1);

    //ArgCallback<ImageState> handler;
    //Callback onTouchDown;
    //Callback onTouchUp;
    //Callback onClick;
    // onTouchDown is called on left click if the cursor is over the image
    void(*onTouchDown)() = 0;

    // onTouchUp is called after releasing left click if onTouchDown was called
    void(*onTouchUp)() = 0;

    // onClick is called if onTouchDown was called and the cursor is still over the image
    void(*onClick)() = 0;

    static UIImage *get_held(int id);
    static UIImage *get_held();
    bool is_within(Vector2 pos);
    Vector2 calc_world_pos();

    inline int get_render_order() { return renderOrder; }
    void set_render_order(int order);

    // should be changed to all images with callbacks registered
    static const std::vector<UIImage *> &get_raycastables() { return *images; }
    
    UIImage(const UIImage &) = delete;
    void operator=(const UIImage &) = delete;
};