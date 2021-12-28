#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/group.hpp>
#include <Galaxy/init.hpp>

#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Scene/scene.hpp>

#include <Renderer/internal.hpp>
#include "uiobject.hpp"

namespace
{
    //std::unique_ptr<Mesh> squareMesh;
    AssetRef<Shader> tintShader, colShader;
    //std::unique_ptr<Shader> tintShader, colShader;
    //std::unique_ptr<Shader> colShader;
    AssetRef<Mesh> squareMesh;
}
Mesh *UIImage::mesh() { return squareMesh.get(); }
Shader *UIImage::shader() { return (texture ? tintShader : colShader).get(); }

auto UIImage::images = std::make_unique<std::vector<UIImage *>>();
//UIImage *currentHeldImg;
//UIImage *heldImages;
//std::map<int, UIImage*> heldImages;
std::unique_ptr<UIImage*[]> heldImages = std::make_unique<UIImage*[]>(10);

UIImage *UIImage::get_held(int id) { return heldImages[id]; }
UIImage::UIImage(Texture *texture) : texture(texture)
{
    //rendererID = InternalRenderer::add_renderer(this);
    rendererID = UIObject::add_image(this);
    imageID = images->size();
    images->push_back(this);

    scene = Scene::activeScene;
    if (scene)
        scene->imgInstances.push_back(this);
}
UIImage::~UIImage()
{
    UIObject::remove(rendererID);
    if (scene) scene->remove_inst(this);

    for (int i = 0; i < 10; ++i)
        if (heldImages[i] == this)
            heldImages[i] = nullptr;
        
    //if (currentHeldImg == this) currentHeldImg = nullptr;

    UIImage *&img = images->back();
    std::swap((*images)[imageID], img);
    img->imageID = imageID;
    images->pop_back();
}
void UIImage::set_render_order(int order)
{
    renderOrder = order;
    UIObject::sortObjects = 1;
}
Vector2 UIImage::calc_world_pos()
{
    if (group)
    {
        return pos*Renderer::reverseAspect + anchor*group->world_scale() + group->world_pos();
    }
    else return pos*Renderer::reverseAspect + anchor;
    //return pos*Renderer::reverseAspect + (keepInBounds ? anchor*Renderer::reverseAspect : anchor);
}
static bool is_within(Vector2 a, Vector2 b, Vector2 scale)
{
    Vector2 radius = scale * 0.5f;
    Vector2 bottomLeft = b-radius;
    Vector2 topRight = b+radius;

    return Math::within(a.x, bottomLeft.x, topRight.x) && Math::within(a.y, bottomLeft.y, topRight.y);
}
bool UIImage::is_within(Vector2 pos)
{
    return ::is_within(pos, calc_world_pos(), scale*Renderer::reverseAspect);
}
static void init()
{
    //squareMesh = std::unique_ptr<Mesh>(Mesh::from_obj(Assets::gasset_path()+"/models/square.obj"));
    squareMesh = Mesh::from_obj(Assets::gasset_path()+"/models/square.obj");

    tintShader = Shader::load(Assets::gasset_path()+SHADER_FOLDER+"/tint.shader");
    colShader = Shader::load(Assets::gasset_path()+SHADER_FOLDER+"/color.shader");
    //tintShader = std::make_unique<Shader>(Assets::gasset_path()+SHADER_FOLDER+"/tint.shader");
    //colShader = std::make_unique<Shader>(Assets::gasset_path()+SHADER_FOLDER+"/color.shader");
}
INIT_FUNC(init);