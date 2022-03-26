#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/ECS/ecs.hpp>
class UIImage;
class UIText;
class Signal;
class Object;
class Material;
struct TransformECS;
class Texture;
class Mesh;
struct Uniform;

// ObjRendererECS & RendererSystem OUTDATED
//struct ObjRendererECS
//{
//    Material *mat = 0;
//    Mesh *mesh = 0;
//    Vector3 i_minBounds, i_maxBounds;
//    bool dirty = 1;
//};

//class RendererSystem : public System<RendererSystem, ObjRendererECS, TransformECS>
//{
//public:
//    void draw(ObjRendererECS &renderer, TransformECS &transform);
//};

class Renderer
{
private:
    static void bind_material(Material *mat);
    static void bind_uniforms(std::unordered_map<int, Uniform> &uniforms);
    friend class RendererSystem;
public:
    static int screenWidth, screenHeight;
    static Vector2 aspectRatio, reverseAspect;

    static Signal& aspect_ratio_changed();
    static Signal& pre_render();
    static Signal& post_simulation();
    static Signal& post_render();

    static void set_clear_color(float r, float g, float b, float a);
    
    // internal
    static void clear();
    static void draw_all(bool fireEvents=0);
    static void draw(Object &obj);
    static void draw(UIImage &img);
    static void draw(UIText &img);
    static void fix_aspect(int width, int height);
};