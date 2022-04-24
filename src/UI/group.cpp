#include <Galaxy/UI/group.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Scene/scene.hpp>

#include <internalinit.hpp>

auto UIGroup::aspectRatio = new UIGroup();
auto UIGroup::safeArea = new UIGroup();

Vector2 UIGroup::calc_pos()
{
    // todo: check this equation
    Vector2 worldPos = pos*Renderer::reverseAspect + anchor*world_scale();
    if (m_parent) return m_parent->world_pos() + worldPos;
    else return worldPos;
}
Vector2 UIGroup::calc_scale()
{
    if (m_parent) return m_parent->world_scale() * scale;
    else return scale;
}
Vector2 UIGroup::calc_aspect(Vector2 worldScale)
{
    Vector2 scale = worldScale*Renderer::aspectRatio;
    if (scale.y > scale.x) return Vector2(1,  scale.y / scale.x);
    else return Vector2(scale.x / scale.y, 1);
}
Vector2 UIGroup::world_pos() { return calc_pos(); }
Vector2 UIGroup::world_scale() { return calc_scale(); }
Vector2 UIGroup::world_aspect() { return calc_aspect(world_scale()); }

//Vector2 UIGroup::world_pos() { return m_worldPos; }
//Vector2 UIGroup::world_scale() { return m_worldScale; }
//Vector2 UIGroup::world_aspect() { return calc_aspect(m_worldScale); }
void UIGroup::cache()
{
    //m_worldPos = calc_pos();
    //m_worldScale = calc_scale();
}
void UIGroup::parent(UIGroup *g)
{
    m_parent = g;
}
UIGroup* UIGroup::create()
{
    UIGroup *g = new UIGroup();
    g->scene = Scene::activeScene;
    if (g->scene) g->sceneID = g->scene->add_inst(g, Scene::GROUP);
    return g;
}
void UIGroup::destroy(UIGroup *g)
{
    delete g;
}
UIGroup::~UIGroup()
{
    if (scene) scene->remove_inst(sceneID);
}