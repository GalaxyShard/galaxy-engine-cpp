#pragma once
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/ECS/ecs.hpp>
class UIImage;
class UIText;
class Signal;
class Object;
class Object2D;
class Material;
struct Uniform;

class Renderer
{
private:
    static void bind_material(Material *mat);
    static void bind_uniforms(std::unordered_map<int, Uniform> &uniforms);
    static void draw_square(Vector2 pos, Vector2 scale=Vector2(0.05f,0.05f)*reverseAspect);
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
    static void draw(Object2D &obj);
    static void draw(UIImage &img);
    static void draw(UIText &img);
    static void fix_aspect(int width, int height);
};