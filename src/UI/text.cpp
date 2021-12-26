#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/group.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Renderer/renderer.hpp>
#include <Renderer/internal.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/init.hpp>

#include "uiobject.hpp"
#include <iostream>
//static Shader *textShader;
static std::unique_ptr<Shader> textShader;
Shader *UIText::shader() { return textShader.get(); }

UIText::UIText(std::string text, Font *font) : text(text), font(font)
{
    rendererID = UIObject::add_text(this);

    mesh = std::make_unique<Mesh>();
    build_mesh();
    mesh->initialize_mesh();

    scene = Scene::activeScene;
    if (scene) scene->textInstances.push_back(this);
}
UIText::~UIText()
{
    UIObject::remove(rendererID);
    if (scene) scene->remove_inst(this);
}
void UIText::set_render_order(int order)
{
    renderOrder = order;
    UIObject::sortObjects = 1;
}

void UIText::refresh()
{
    build_mesh();
    mesh->refresh_mesh();
}
static float xcoord(int pixel, float size) { return pixel / size + (1.f / (size * 2.f)); }
static float ycoord(int pixel, float size) { return xcoord(size-pixel, size); }
void UIText::build_mesh()
{
    auto &tris = mesh->tris;
    auto &verts = mesh->verts;

    verts = std::vector<Vertex>(text.size()*4);
    //verts.clear();
    //verts.resize(text.size());
    //if (verts.size() < text.size());
    //verts.reserve(text.size());

    tris = std::vector<unsigned int>(text.size()*6);
    tris.clear();
    //tris.resize(text.size());
    tris.reserve(text.size());

    // scale is bounding box
    //const int fontSize = 72*2.5f, lineHeight = 72;
    const int fontSize = 72, lineHeight = 72;

    // converts pixels to screen pos
    //float toScreen = scale.x/fontSize * 2.5f / text.size();
    //float toScreen = scale.x/fontSize * 2.5f / charsInLine;
//
    //float toScreen, charSize;
    //
    //if (scale.y * text.size() < scale.x) toScreen = scale.y / fontSize * 2.5f;
    //if (scale.y * text.size() < scale.x * 2.5f)
    //if (scale.y * text.size() < scale.x)
    //{
        //charSize = scale.y;
        //toScreen = 1/scale.y;
        //toScreen = scale.y / fontSize;
    //}
    //else
    //{
        //charSize = scale.x*2.5 / text.size();
        //charSize = scale.x / text.size();
        //toScreen = scale.x/fontSize / text.size();
        //toScreen = (scale.x / text.size());
        //toScreen = scale.x/fontSize*2.5f / text.size();
    //}
    //else toScreen = scale.x/fontSize * 2.5f / visibleChars;
    //toScreen = scale.x/fontSize * 2.5f / text.size();
    

    float xadv = 0;
    float line = 0;

    // convert to pixel space instead of screen bc scale is already screen
    //float negRadiusY = -lineHeight*toScreen/2;
//
    //float negRadiusX = -scale.x/2.f*(1/toScreen);
    //float negRadiusX = 0;
    //float negRadiusX = text.size();
    //float negRadiusY = -lineHeight/2;
    //Vector2 pivotScreen = Vector2(
    //    //-negRadiusX * (pivot.x),
    //    -negRadiusX * (pivot.x-1),
    //    negRadiusY * (pivot.y+1)
    //);
    //Vector2 pivotScreen = Vector2(
    //    //-charSize,
    //    //charSize/2
    //     -(charSize)/2 * (pivot.x+1) * (1/toScreen),
    //     (charSize)/2 * (pivot.y+1) * (1/toScreen)
    //);
    //Vector2 pivotScreen = Vector2(
    //     - charSize,
    //     - charSize
    //) * (1/toScreen);
    for (char c : text)
    {

        //if (c == '\n')
        //{
        //    line += lineHeight;
        //    xadv = 0;
        //    continue;
        //}
//
        // TODO: apply width and height
        // Everything is relative to font size
        // xOffset is the offset from the xAdvance for each character
        // yOffset is the offset from the top of the line for each character
    
        auto &data = font->get_char(c);
        int vert0 = verts.size();

        if (c == ' ')
        {
            xadv += data.xadv;
            continue;
        }
        //float w = data.width * toScreen;
        //float h = data.height * toScreen;
        //float xoff = (data.xoff + xadv + pivotScreen.x) * toScreen;
  //
        //// yoff is set to a value from top left, this offsets from bottom left
        //float yoff = (data.yoff+line + pivotScreen.y) * toScreen;

        float w = data.width / fontSize;
        float h = data.height / fontSize;
        float xoff = (data.xoff + xadv) / fontSize;
  
        // yoff is set to a value from top left, this offsets from bottom left
        float yoff = (data.yoff + line) / fontSize;

        verts.push_back(Vertex(Vector3(xoff,   -(yoff)),   Vector2(data.xCoord0, data.yCoord0)));
        verts.push_back(Vertex(Vector3(xoff+w, -(yoff)),   Vector2(data.xCoord1, data.yCoord0)));
        verts.push_back(Vertex(Vector3(xoff,   -(yoff+h)), Vector2(data.xCoord0, data.yCoord1)));
        verts.push_back(Vertex(Vector3(xoff+w, -(yoff+h)), Vector2(data.xCoord1, data.yCoord1)));

        tris.push_back(vert0 + 0);
        tris.push_back(vert0 + 2);
        tris.push_back(vert0 + 1);

        tris.push_back(vert0 + 1);
        tris.push_back(vert0 + 2);
        tris.push_back(vert0 + 3);
        //xadv += data.xadv * toScreen;
        xadv += data.xadv;
    }
}
Vector2 UIText::calc_world_pos()
{
    if (group)
    {
        return pos*Renderer::reverseAspect + anchor*group->world_scale() + group->world_pos();
    }
    else return pos*Renderer::reverseAspect + anchor;
    //return pos*Renderer::reverseAspect + (keepInBounds ? anchor*Renderer::reverseAspect : anchor);
}

static void init()
{
    //textShader = new Shader(Assets::gasset_path()+"/shaders/text.shader");
    textShader = std::make_unique<Shader>(Assets::gasset_path()+SHADER_FOLDER+"/text.shader");
}
INIT_FUNC(init);