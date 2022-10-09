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

    unsigned int rendererID, sceneID;
    int renderOrder = 0;

    void build_mesh();
    UIText() = default;
    
    Vector2 calc_world_pos();
    Vector2 world_scale();

    friend class Renderer;
    friend struct UIObject;
    friend class Scene;
public:
    std::string str;
    UIGroup *group = 0;
    static UIText* create(std::string text, Font *font = Font::defaultFont);
    static void destroy(UIText *text);
    ~UIText();

    Vector2 pos;
    Vector2 scale = Vector2(1,1);
    Vector2 anchor;
    Vector2 pivot;
    // not implemented
    //bool wrap = 0;
    //bool scaleToFit = 1; // scales to the max size that can fit

    inline int render_order() { return renderOrder; }
    void render_order(int order);
    // Call when text is changed
    void refresh();

    inline const Mesh *get_mesh() const { return mesh.get(); }
};