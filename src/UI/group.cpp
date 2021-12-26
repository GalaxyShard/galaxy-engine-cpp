#include <Galaxy/UI/group.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Scene/scene.hpp>
#include <internalinit.hpp>

std::unique_ptr<UIGroup> UIGroup::aspectRatio, UIGroup::safeArea;

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
    if (scene) scene->groupInstances.push_back(this);
}
UIGroup::~UIGroup()
{
    if (scene) scene->remove_inst(this);
}

void init()
{
    UIGroup::aspectRatio = std::make_unique<UIGroup>();
    UIGroup::safeArea = std::make_unique<UIGroup>();
}  
INTERNAL_INIT_FUNC(init);