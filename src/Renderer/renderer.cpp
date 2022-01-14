#include <Galaxy/Renderer/renderer.hpp>
#include <debug.hpp>
#include <internalinit.hpp>
#include <Renderer/buffer.hpp>
//#include <Renderer/internal.hpp>
#include <UI/uiobject.hpp>

#include <Galaxy/event.hpp>
#include <Galaxy/object.hpp>

#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Renderer/material.hpp>
#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Renderer/camera.hpp>

#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/group.hpp>

namespace
{
    void init()
    {
        int w, h;
        #if OS_MOBILE
            glfmGetDisplaySize(glfmDisplay, &w, &h);
        #else
            //glfwGetFramebufferSize(glfwGetCurrentContext(), &w, &h);
            glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
        #endif
        Renderer::fix_aspect(w, h);

        // probably should apply aspect ratio to projection
        Camera::main->projection = Matrix4x4::ortho(-1.f, 1.f, -1.f, 1.f, -100000.f, 100000.f, 0);
    }
    INTERNAL_INIT_FUNC(init);

    //Event *preRender = new Event();
    //Event *postRender = new Event();
    auto preRender = std::make_unique<Event>();
    auto postRender = std::make_unique<Event>();
}
int Renderer::screenWidth, Renderer::screenHeight;
Vector2 Renderer::aspectRatio, Renderer::reverseAspect;

void Renderer::fix_aspect(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    if (h > w)
    {
        aspectRatio = Vector2(1,  (float)h / (float)w);
        reverseAspect = Vector2(1, (float)w / (float)h);
    }
    else
    {
        aspectRatio = Vector2((float)w / (float)h, 1);
        reverseAspect = Vector2((float)h / (float)w, 1);
    }
    UIGroup::aspectRatio->scale = reverseAspect;

#if OS_MOBILE
    GLCall(glViewport(0, 0, w, h));
#endif
}
void Renderer::bind_material(Material *mat)
{
    for (const auto &v : mat->uniforms)
    {
        const int i = v.first;
        auto &val = v.second.value;
        auto &type = v.second.type;

        if (type == Uniform::FLOAT) glUniform1f(i, val.v);
        else if (type == Uniform::VEC2) glUniform2fv(i, 1, &val.v2.x);
        else if (type == Uniform::VEC3) glUniform3fv(i, 1, &val.v3.x);
        else if (type == Uniform::VEC4) glUniform4fv(i, 1, &val.v4.x);
        else if (type == Uniform::INT) glUniform1i(i, val.i);
        else if (type == Uniform::MAT4x4) glUniformMatrix4fv(i, 1, false, val.m4x4.transpose().value_ptr());
        else assert(false); // Case not implemented
        assert(gl_check_errors());
    }

}
void Renderer::draw(Object &obj)
{
    obj.mesh->varray->bind();

    Material &mat = *obj.material;
    Shader &shader = *mat.shader;
    Texture *tex = mat.mainTex;
    shader.bind();
    
    Vector3 filteredAspect = Camera::main->isPerspective ? Vector3(1,1,1) : Vector3(reverseAspect.x, reverseAspect.y, 1);

    Matrix4x4 model =
        Matrix4x4::translate(obj.position * filteredAspect)
        * Matrix4x4::rotate(obj.rotation)
        * Matrix4x4::scale(obj.scale * filteredAspect);


    // Rotate camera after translating
    // The transpose of a rotation is equal to the inverse
    Matrix4x4 view = Matrix4x4::rotate(Camera::main->rotation).transpose()
        * Matrix4x4::translate(-Camera::main->position);

    shader.set_uniform_mat4x4("u_mvp", Camera::main->projection * view * model);

    bind_material(obj.material);
    if (tex)
    {
        tex->bind();
        shader.set_uniform1i("u_tex", tex->get_slot());
    }
    GLCall(glDrawElements(GL_TRIANGLES, obj.mesh->tris.size(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::draw(UIImage &img)
{
    UIImage::mesh()->varray->bind();
    Shader *shader = img.material ? img.material->shader : img.shader();
    shader->bind();

    Vector2 scale = img.scale * reverseAspect;
    Vector2 world_pos = img.calc_world_pos();
    
    Matrix4x4 model = Matrix4x4(
        scale.x, 0,       0,       world_pos.x,
        0,       scale.y, 0,       world_pos.y,
        0,       0,       1,       0,
        0,       0,       0,       1
    );
    shader->set_uniform_mat4x4("u_mvp", model);
    shader->set_uniform4f("u_color", img.tint);

    if (img.material) bind_material(img.material);
    if (img.texture)
    {
        img.texture->bind();
        shader->set_uniform1i("u_tex", img.texture->get_slot());
    }
    
    GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
}
void Renderer::draw(UIText &text)
{
    UIText::shader()->bind();
    float charSize = Math::min(text.scale.x/text.text.size()*2.5f, text.scale.y);
    float textWidth = charSize*text.text.size()/2.5f;

    //Vector2 filteredRatio = text.keepInBounds ? reverseAspect : Vector2(1,1);

    //Vector2 world_pos = text.calc_world_pos()
    //    + (text.scale/2*Vector2(text.pivot.x, text.pivot.y))*reverseAspect
    //    + (Vector2(-textWidth, charSize)/2)*reverseAspect
    //    + (Vector2(-textWidth, -charSize)/2*text.pivot)*reverseAspect;
    Vector2 world_pos = text.calc_world_pos()
        + (text.scale/2*Vector2(text.pivot.x, text.pivot.y)
        + Vector2(-textWidth, charSize)/2
        + Vector2(-textWidth, -charSize)/2*text.pivot)*reverseAspect;


    Vector2 scale = Vector2(charSize,charSize) * reverseAspect;
    Matrix4x4 model = Matrix4x4(
        scale.x, 0,       0,       world_pos.x,
        0,       scale.y, 0,       world_pos.y,
        0,       0,       1,       0,
        0,       0,       0,       1
    );
    text.shader()->set_uniform_mat4x4("u_mvp", model);
    text.mesh->varray->bind();

    text.font->fontTex->bind();
    text.shader()->set_uniform1i("u_tex", text.font->fontTex->get_slot());


    GLCall(glDrawElements(GL_TRIANGLES, text.mesh->tris.size(), GL_UNSIGNED_INT, nullptr));
}
void Renderer::set_clear_color(float r, float g, float b, float a)
{
    GLCall(glClearColor(r, g, b, a));
}
void Renderer::clear()
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}
void Renderer::draw_all(bool fireEvents)
{
    if (fireEvents) preRender->fire();

    //auto objectsCopy = *Object::allObjects;
    if (Camera::main->isPerspective)
    {
        GLCall(glEnable(GL_DEPTH_TEST));
    }
    if (Object::sortObjects)
    {
        auto &objs = Object::allObjects;
        std::sort(objs->begin(), objs->end(), [](Object *a, Object *b)
        { return a->renderOrder < b->renderOrder; });
        int id = 0;
        for (auto &obj : *objs)
        {
            obj->objectIndex = id;
            ++id;
        }
        Object::sortObjects = 0;
    }
    for (Object *obj : *Object::allObjects) draw(*obj);

// glfwSetRefreshCallback for resizing
/*
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
    glDepthMask(true) required for depth, enabled by default?

    Enable backface culling
*/
    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glCullFace(GL_BACK));
    GLCall(glFrontFace(GL_CCW)); // clockwise vertex ordering
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
    GLCall(glDisable(GL_DEPTH_TEST));
    
    auto &uiObjs = UIObject::uiObjects;
    if (UIObject::sortObjects)
    {
        std::sort(uiObjs->begin(), uiObjs->end(), [](UIObject *a, UIObject *b)
        { return a->render_order() < b->render_order(); });

        for (int i = 0; i < uiObjs->size(); ++i)
        {
            (*uiObjs)[i]->renderer_id() = i;
        }
        UIObject::sortObjects = 0;
    }
    for (auto &ui : *uiObjs)
    {
        if (ui->type == UIObject::UIIMAGE) draw(*((UIImage*)ui->data));
        else if (ui->type == UIObject::UITEXT) draw(*((UIText*)ui->data));
    }
    if (fireEvents) postRender->fire();
}
Signal &Renderer::pre_render() { return *preRender->signal; }
Signal &Renderer::post_render() { return *postRender->signal; }