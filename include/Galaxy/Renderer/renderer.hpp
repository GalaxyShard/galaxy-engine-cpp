#pragma once
#include <Galaxy/Math/vector.hpp>
class UIImage;
class UIText;
//class Event;
//class EventListener;
class Signal;
class Object;
class Material;
typedef void (*empty_func)();

class Renderer
{
    private:
        static void bind_material(Material *mat);
    public:
        static int screenWidth, screenHeight;
        static Vector2 aspectRatio, reverseAspect;

        static Signal& pre_render();
        static Signal& post_render();

        // internal
        static void set_clear_color(float r, float g, float b, float a);
        static void clear();
        static void draw_all(bool fireEvents=0);
        static void draw(Object &obj);
        static void draw(UIImage &img);
        static void draw(UIText &img);
        static void fix_aspect(int width, int height);
};