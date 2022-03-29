#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/group.hpp>
#include <Galaxy/init.hpp>

#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Scene/scene.hpp>

#include "uiobject.hpp"

namespace
{
    AssetRef<Shader> tintShader, colShader;
    AssetRef<Mesh> squareMesh;
}
Mesh *UIImage::mesh() { return squareMesh.get(); }
Shader *UIImage::shader() { return (texture ? tintShader : colShader).get(); }

auto UIImage::images = std::vector<UIImage *>();
UIImage** heldImages = new UIImage*[10];
int lastTouchID = 0;

UIImage *UIImage::get_held(int id) { return heldImages[id]; }
UIImage *UIImage::get_held() { return heldImages[lastTouchID]; }

UIImage* UIImage::create(Texture *texture)
{
    UIImage *image = new UIImage();
    image->texture = texture;
    image->rendererID = UIObject::add_image(image);
    image->imageID = images.size();
    images.push_back(image);

    image->scene = Scene::activeScene;
    if (image->scene)
        image->sceneID = image->scene->add_inst(image, Scene::IMG);
    
    return image;
}
void UIImage::destroy(UIImage *image)
{
    delete image;
}
UIImage::~UIImage()
{
    UIObject::remove(rendererID);
    if (scene) scene->remove_inst(sceneID);

    for (int i = 0; i < 10; ++i)
        if (heldImages[i] == this)
            heldImages[i] = nullptr;

    UIImage *img = images.back();
    if (img==this)
    {
        images.pop_back();
        return;
    }
    //std::swap((images)[imageID], img);
    images[imageID] = std::move(images.back());
    
    img->imageID = imageID;
    images.pop_back();
}
void UIImage::render_order(int order)
{
    renderOrder = order;
    UIObject::sortObjects = 1;
}
Vector2 UIImage::calc_world_pos()
{
    if (group) return pos*Renderer::reverseAspect + anchor*group->world_scale() + group->world_pos();
    else return pos*Renderer::reverseAspect + anchor;
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
void init_image()
{
    squareMesh = Mesh::load_obj(Assets::gpath()+"/models/square.obj");

    tintShader = Shader::load(Assets::gpath()+SHADER_FOLDER+"/tint.shader");
    colShader = Shader::load(Assets::gpath()+SHADER_FOLDER+"/color.shader");
    memset(heldImages, 0, sizeof(UIImage*)*10);
}