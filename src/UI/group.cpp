#include <Galaxy/UI/group.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Scene/scene.hpp>

#include <internalinit.hpp>

auto UIGroup::aspectRatio = new UIGroup();
auto UIGroup::safeArea = new UIGroup();

Vector2 UIGroup::world_pos()
{
    Vector2 worldPos = pos*Renderer::reverseAspect + anchor*world_scale();
    if (parent) return parent->world_pos() + worldPos;
    else return worldPos;
}
Vector2 UIGroup::world_scale()
{
    if (parent) return parent->world_scale() * scale;
    else return scale;
}
UIGroup::UIGroup()
{
    scene = Scene::activeScene;
    //if (scene) scene->groupInstances.push_back(this);
    if (scene) scene->add_inst(this, Scene::GROUP);
}
UIGroup::~UIGroup()
{
    if (scene) scene->remove_inst(sceneID);
    //if (scene) scene->remove_inst(this);
}