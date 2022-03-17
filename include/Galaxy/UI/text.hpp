#pragma once
#include <string>
#include <vector>
#include <Galaxy/UI/font.hpp>
#include <Galaxy/Renderer/mesh.hpp>
class Shader;
class Scene;
class UIGroup;

class UIText
{
private:
    static Shader *shader();

    std::unique_ptr<Mesh> mesh;
    Font *font;
    Scene *scene;

    unsigned int rendererID;
    int renderOrder = 0;

    void build_mesh();

    friend class Renderer;
    friend struct UIObject;
public:
    std::string text;
    UIGroup *group = 0;
    UIText(std::string text, Font *font = Font::defaultFont);
    ~UIText();

    Vector2 pos;
    Vector2 scale = Vector2(1,1);
    Vector2 anchor = Vector2(-1,-1);
    Vector2 pivot = Vector2(1, 1);
    // not implemented
    bool wrap = 0;
    bool scaleToFit = 1; // scales to the max size that can fit

    inline int get_render_order() { return renderOrder; }
    void set_render_order(int order);
    // Should be called when text is changed
    void refresh();

    Vector2 calc_world_pos();
};