#include <Galaxy/Renderer/renderer.hpp>

#include <Galaxy/Events/event.hpp>
#include <Galaxy/object.hpp>

#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Renderer/material.hpp>
#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Renderer/camera.hpp>

#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/group.hpp>

#include <Galaxy/Math/time.hpp>
#include <Galaxy/Physics/physics.hpp>


#include <Physics/combinations.hpp>
#include <gldebug.hpp>
#include <internalinit.hpp>
#include <Renderer/buffer.hpp>
#include <UI/uiobject.hpp>

#include <array>
namespace
{
    RendererSystem *renderSystem;
    void init()
    {
        int w, h;
        #if USE_GLFM
            glfmGetDisplaySize(glfmDisplay, &w, &h);
        #endif
        #if USE_GLFW
            glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
        #endif

        Camera::main->projection = Matrix4x4::ortho(-1.f, 1.f, -1.f, 1.f, -100000.f, 100000.f, 0);
        Renderer::fix_aspect(w, h);
        
        renderSystem = ECSManager::main->reg_sys<RendererSystem>();
    }
    INTERNAL_INIT_FUNC(init);

    auto aspectChanged = std::make_unique<Event>();
    auto preRender = std::make_unique<Event>();
    auto postSimulation = std::make_unique<Event>();
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

#if USE_GLFM
    //printf("size: %d, %d\n",w,h);
    GLCall(glViewport(0, 0, w, h));
    //GLCall(glViewport(0, 0, 1200, 600));
#endif
    aspectChanged->fire();
    
}

void Renderer::bind_uniforms(std::unordered_map<int, Uniform> &uniforms)
{
    for (const auto &[i, uniform] : uniforms)
    {
        //const int i = v.first;
        auto &val = uniform.value;
        auto &type = uniform.type;

        if (type == Uniform::FLOAT) glUniform1f(i, val.v);
        else if (type == Uniform::VEC2) glUniform2fv(i, 1, &val.v2.x);
        else if (type == Uniform::VEC3) glUniform3fv(i, 1, &val.v3.x);
        else if (type == Uniform::VEC4) glUniform4fv(i, 1, &val.v4.x);
        else if (type == Uniform::INT) glUniform1i(i, val.i);
        else if (type == Uniform::MAT4x4) glUniformMatrix4fv(i, 1, false, val.m4x4.transpose().value_ptr());
        //else assert(false && "Case not implemented");
        else throw("Case not implemented");
        GLCall();
    }

}
void Renderer::bind_material(Material *mat)
{
    bind_uniforms(mat->uniforms);
}
static int objectsCulled = 0;
//static bool shouldCull(ObjRendererECS &renderer, TransformECS &transform)
//{
//    // very basic inaccurate frustum culling
//    Vector3 normal = Matrix3x3::rotate(Camera::main->rotation) * Vector3(0,0,-1);
//    Vector3 toWorld = transform.pos - Camera::main->position;
//    if (Vector3::dot(normal, renderer.mesh->aabbMin + toWorld) < 0 
//        && Vector3::dot(normal, renderer.mesh->aabbMax + toWorld) < 0)
//    {
//        ++objectsCulled;
//        return true;
//    }
//    return false;
//}
Matrix4x4 add_r_c(Matrix3x3 m)
{
    const float *p = m.value_ptr();
    return Matrix4x4(
        p[0], p[1], p[2], 0,
        p[3], p[4], p[5], 0,
        p[6], p[7], p[8], 0,
        0,    0,    0,    1
    );
}
void RendererSystem::draw(ObjRendererECS &renderer, TransformECS &transform)
{
    Matrix3x3 rotation = Matrix3x3::rotate(transform.i_rotation);

    // Rotate camera after translating
    // The transpose of a rotation is equal to the inverse
    Matrix4x4 view = Matrix4x4::rotate(Camera::main->rotation).transpose()
        * Matrix4x4::translate(-Camera::main->position);

    if (Camera::main->isPerspective)
    {
        //Vector3 min = renderer.mesh->aabbMin+transform.pos;
        //Vector3 max = renderer.mesh->aabbMax+transform.pos;
        //Vector3 min = renderer.mesh->aabbMin;
        //Vector3 max = renderer.mesh->aabbMax;
        Vector3 &min = renderer.i_minBounds;
        Vector3 &max = renderer.i_maxBounds;
        if (transform.dirty)
        {
            min = renderer.mesh->aabbMin;
            max = renderer.mesh->aabbMax;
            transform.dirty = 0;
            Vector3 obbPoints[8] = {
                Vector3(min.x, min.y, min.z),
                Vector3(min.x, max.y, min.z),
                Vector3(max.x, min.y, min.z),
                Vector3(max.x, max.y, min.z),
                Vector3(max.x, max.y, max.z),
                Vector3(max.x, min.y, max.z),
                Vector3(min.x, max.y, max.z),
                Vector3(min.x, min.y, max.z)
            };
            min = Vector3(Math::INF, Math::INF, Math::INF);
            max = Vector3(-Math::INF, -Math::INF, -Math::INF);
            for (int i = 0; i < 8; ++i)
            {
                obbPoints[i] = rotation*(obbPoints[i]*transform.scale()) + transform.pos();
                for (int j = 0; j < 3; ++j)
                {
                    min[j] = std::min(min[j], obbPoints[i][j]);
                    max[j] = std::max(max[j], obbPoints[i][j]);
                }
            }
        }
        
        auto checkPlane = [&](Vector4 frustumPlane)
        {
            frustumPlane = frustumPlane / ((Vector3)frustumPlane).magnitude();
            float planeConstant = frustumPlane.w;
            Vector3 vertex;
            for (int i = 0; i < 3; ++i)
            {
                if (frustumPlane[i] < 0) vertex[i] = min[i];
                else vertex[i] = max[i];
            }
            if (Vector3::dot(frustumPlane, vertex)+planeConstant < 0)
            {
                ++objectsCulled;
                return 1;
            }
            return 0;
        };
        Matrix4x4 viewProj = (Camera::main->projection * view);
        if (checkPlane(viewProj[3] + viewProj[0])
            || checkPlane(viewProj[3] - viewProj[0])
            || checkPlane(viewProj[3] - viewProj[1])
            || checkPlane(viewProj[3] + viewProj[1])
            || checkPlane(viewProj[2])
            || checkPlane(viewProj[3] - viewProj[2]))
                return;
    }
    
    Material &mat = *renderer.mat;
    Shader &shader = *mat.shader;
    Texture *tex = mat.mainTex;

    Vector3 filteredAspect = Camera::main->isPerspective ? Vector3(1,1,1) : Vector3(Renderer::reverseAspect.x, Renderer::reverseAspect.y, 1);

    Matrix4x4 rotation4x4 = add_r_c(rotation);
   
    Matrix4x4 model =
        Matrix4x4::translate(transform.pos() * filteredAspect)
        * rotation4x4
        * Matrix4x4::scale(transform.scale() * filteredAspect);

    Matrix4x4 mvp = Camera::main->projection * view * model;

    renderer.mesh->varray->bind();
    shader.bind();
    Renderer::bind_material(renderer.mat);
    shader.set_uniform_mat4x4("u_mvp", mvp);
    shader.set_uniform_mat4x4("u_model", model);
    shader.set_uniform_mat4x4("u_rotation", rotation4x4);
    shader.set_uniform3f("u_camPos", Camera::main->position);
    if (tex)
    {
        tex->bind();
        shader.set_uniform1i("u_tex", tex->get_slot());
    }
    //GLCall(glDrawElements(GL_TRIANGLES, renderer.mesh->tris.size(), GL_UNSIGNED_SHORT, nullptr));
    GLCall(glDrawElements(GL_TRIANGLES, renderer.mesh->tris.size(), GL_UNSIGNED_INT, nullptr));

}
void Renderer::draw(Object &obj)
{
    // hack: rewrite this
    ObjRendererECS renderer = ObjRendererECS{.mat=obj.material, .mesh=obj.mesh};
    //TransformECS transform = TransformECS{.pos=obj.position,.scale=obj.scale,.rotation=obj.rotation};
    TransformECS transform = TransformECS(obj.position,obj.scale,obj.rotation);
    RendererSystem().draw(renderer, transform);
}

//void Renderer::draw(Object &obj)
//{
//    obj.mesh->varray->bind();
//
//    Material &mat = *obj.material;
//    Shader &shader = *mat.shader;
//    Texture *tex = mat.mainTex;
//    shader.bind();
    //
//    Vector3 filteredAspect = Camera::main->isPerspective ? Vector3(1,1,1) : Vector3(reverseAspect.x, reverseAspect.y, 1);
//
//    Matrix4x4 rotation = Matrix4x4::rotate(obj.rotation);
//    Matrix4x4 model =
//        Matrix4x4::translate(obj.position * filteredAspect)
//        * rotation
//        * Matrix4x4::scale(obj.scale * filteredAspect);
//
//
//    // Rotate camera after translating
//    // The transpose of a rotation is equal to the inverse
//    Matrix4x4 view = Matrix4x4::rotate(Camera::main->rotation).transpose()
//        * Matrix4x4::translate(-Camera::main->position);
//
//
//    bind_material(obj.material);
//    shader.set_uniform_mat4x4("u_mvp", Camera::main->projection * view * model);
//    shader.set_uniform_mat4x4("u_model", model);
//    shader.set_uniform_mat4x4("u_rotation", rotation);
//    shader.set_uniform3f("u_camPos", Camera::main->position);
//    if (tex)
//    {
//        tex->bind();
//        shader.set_uniform1i("u_tex", tex->get_slot());
//    }
    //GLCall(glDrawElements(GL_TRIANGLES, obj.mesh->tris.size(), GL_UNSIGNED_INT, nullptr));
//}

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
    //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr));
    //GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr));
}
void Renderer::draw(UIText &text)
{
    UIText::shader()->bind();
    float charSize = Math::min(text.scale.x/text.text.size()*2.5f, text.scale.y);
    float textWidth = charSize*text.text.size()/2.5f;

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


    //GLCall(glDrawElements(GL_TRIANGLES, text.mesh->tris.size(), GL_UNSIGNED_SHORT, nullptr));
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
    if (fireEvents)
    {
        preRender->fire();
        Physics::simulate(Time::delta());
        
        postSimulation->fire();
    }

    if (Camera::main->isPerspective)
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDisable(GL_BLEND));
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
    clear();
    for (Object *obj : *Object::allObjects) draw(*obj);
    renderSystem->run(&RendererSystem::draw, *ECSManager::main);
    //if (objectsCulled)
    //printf("Culled: %d\n", objectsCulled);
    objectsCulled = 0;

/*
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe
*/
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
    if (Camera::main->isPerspective)
    {
        GLCall(glDisable(GL_DEPTH_TEST));
        GLCall(glEnable(GL_BLEND))
    }
    
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
Signal &Renderer::aspect_ratio_changed() { return *aspectChanged->signal; }
Signal &Renderer::pre_render() { return *preRender->signal; }
Signal &Renderer::post_simulation() { return *postSimulation->signal; }
Signal &Renderer::post_render() { return *postRender->signal; }