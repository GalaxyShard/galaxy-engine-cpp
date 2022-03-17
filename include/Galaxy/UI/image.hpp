#pragma once
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Events/callback.hpp>
#include <vector>

class Shader;
class Mesh;
class Scene;
class UIGroup;

class UIImage
{
private:
    static std::unique_ptr<std::vector<UIImage *>> images;
    Scene *scene;

    static Mesh *mesh();
    Shader *shader();

    friend class Renderer;
    friend struct UIObject;
public:
    UIImage(Texture *texture = nullptr);
    ~UIImage();

    Texture *texture = 0;
    Material *material = 0;
    UIGroup *group = 0;

    // onTouchDown is called on click if the cursor is over the image
    // onTouchUp is called after releasing click if onTouchDown was called
    // onClick is called if onTouchDown was called and the cursor is still over the image
    Callback onTouchDown;
    Callback onTouchUp;
    Callback onClick;
    
    Vector2 pos, scale = Vector2(1, 1), anchor = Vector2(-1, -1);
    Vector4 tint = Vector4(1, 1, 1, 1);

private:
    unsigned int imageID, rendererID;
    int renderOrder = 0;
public:

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