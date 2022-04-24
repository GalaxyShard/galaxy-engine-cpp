#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/group.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/init.hpp>
#include <iostream>


#include "uiobject.hpp"
static AssetRef<Shader> textShader;
Shader *UIText::shader() { return textShader.get(); }

UIText* UIText::create(std::string str, Font *font)
{
    UIText *text = new UIText();
    text->font = font;
    text->str = str;
    text->rendererID = UIObject::add_text(text);
    text->mesh = std::make_unique<Mesh>();
    text->build_mesh();
    text->mesh->initialize_mesh();
    text->scene = Scene::activeScene;
    if (text->scene) text->sceneID = text->scene->add_inst(text, Scene::TXT);
    return text;
}
void UIText::destroy(UIText *text)
{
    delete text;
}
UIText::~UIText()
{
    UIObject::remove(rendererID);
    if (scene) scene->remove_inst(sceneID);
}
void UIText::render_order(int order)
{
    renderOrder = order;
    UIObject::sortObjects = 1;
}

void UIText::refresh()
{
    build_mesh();
    mesh->refresh_mesh();
}
Vector2 UIText::calc_world_pos()
{
    if (group) return pos/group->world_aspect()*group->world_scale() + anchor*group->world_scale() + group->world_pos();
    return pos*Renderer::reverseAspect + anchor;
}
Vector2 UIText::world_scale()
{
    if (group) return (scale/group->world_aspect())*group->world_scale();
    return scale*Renderer::reverseAspect;
}
void UIText::build_mesh()
{
    auto &tris = mesh->tris;
    auto &verts = mesh->verts;

    verts.clear();
    verts.reserve(str.size()*4);
    tris.clear();
    tris.reserve(str.size()*6);

    float xadv = 0;
    float line = 0;

    for (char c : str)
    {
        //if (c == '\n')
        //{
        //    line += lineHeight;
        //    xadv = 0;
        //    continue;
        //}

        // Everything is relative to font size
        // xOffset is the offset from the xAdvance for each character
        // yOffset is the offset from the top of the line for each character
    
        auto &data = font->get_char(c);
        unsigned int vert0 = verts.size();

        if (c == ' ')
        {
            xadv += data.xadv;
            continue;
        }
        float charSize = 72;
        //float charSize = data.xadv;
        float w = data.width / charSize;
        float h = data.height / charSize;
        float xoff = (data.xoff + xadv) / charSize;
  
        // yoff is set to a value from top, mesh offsets from bottom left
        float yoff = (data.yoff + line) / charSize;

        verts.push_back(Vertex(Vector3(xoff,   -(yoff)), Vector2(data.xCoord0, data.yCoord0)));
        verts.push_back(Vertex(Vector3(xoff+w, -(yoff)), Vector2(data.xCoord1, data.yCoord0)));
        verts.push_back(Vertex(Vector3(xoff,   -(yoff+h)), Vector2(data.xCoord0, data.yCoord1)));
        verts.push_back(Vertex(Vector3(xoff+w, -(yoff+h)), Vector2(data.xCoord1, data.yCoord1)));

        //verts.push_back(Vertex(Vector3(xoff,   -(yoff)),   Vector2(data.xCoord0, data.yCoord0)));
        //verts.push_back(Vertex(Vector3(xoff+w, -(yoff)),   Vector2(data.xCoord1, data.yCoord0)));
        //verts.push_back(Vertex(Vector3(xoff,   -(yoff+h)), Vector2(data.xCoord0, data.yCoord1)));
        //verts.push_back(Vertex(Vector3(xoff+w, -(yoff+h)), Vector2(data.xCoord1, data.yCoord1)));

        tris.push_back(vert0 + 0);
        tris.push_back(vert0 + 2);
        tris.push_back(vert0 + 1);

        tris.push_back(vert0 + 1);
        tris.push_back(vert0 + 2);
        tris.push_back(vert0 + 3);
        xadv += data.xadv;
    }
    mesh->calculate_bounds();
//    Vector2 min = mesh->aabbMin;
//    Vector2 max = mesh->aabbMax;
//    for (auto &vert : verts)
//    {
//        vert.pos.x = Math::remap(vert.pos.x, min.x, max.x, -0.5f, 0.5f);
//        vert.pos.y = Math::remap(vert.pos.y, min.y, max.y, -0.5f, 0.5f);
//    }
//    mesh->calculate_bounds();
}
void init_text()
{
    textShader = Shader::load(Assets::gpath()+SHADER_FOLDER+"/text.shader");
}