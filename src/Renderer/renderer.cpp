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
void iinit_renderer()
{
    int w, h;
    #if USE_GLFM
        glfmGetDisplaySize(glfmDisplay, &w, &h);
    #endif
    #if USE_GLFW
        glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);
    #endif

    Renderer::fix_aspect(w, h);
}
static auto aspectChanged = new Event();
static auto preRender = new Event();
static auto postSimulation = new Event();
static auto postRender = new Event();
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
    GLCall(glViewport(0, 0, w, h));
#endif
    Camera::main->refresh();
    aspectChanged->fire();
}

void Renderer::bind_uniforms(std::unordered_map<int, Uniform> &uniforms)
{
    for (const auto &[i, uniform] : uniforms)
    {
        auto &val = uniform.value;
        auto &type = uniform.type;

        if (type == Uniform::FLOAT) glUniform1f(i, val.v);
        else if (type == Uniform::VEC2) glUniform2fv(i, 1, &val.v2.x);
        else if (type == Uniform::VEC3) glUniform3fv(i, 1, &val.v3.x);
        else if (type == Uniform::VEC4) glUniform4fv(i, 1, &val.v4.x);
        else if (type == Uniform::INT) glUniform1i(i, val.i);
        else if (type == Uniform::MAT4x4) glUniformMatrix4fv(i, 1, false, val.m4x4.transpose().value_ptr());
        else assert(false && "Case not implemented");
        GLCall();
    }

}
void Renderer::bind_material(Material *mat)
{
    bind_uniforms(mat->uniforms);
}
static Matrix4x4 add_r_c(Matrix3x3 m)
{
    const float *p = m.value_ptr();
    return Matrix4x4(
        p[0], p[1], p[2], 0,
        p[3], p[4], p[5], 0,
        p[6], p[7], p[8], 0,
        0,    0,    0,    1
    );
}
void Renderer::draw(Object &obj)
{
    if (!obj.enabled)
        return;
    Matrix3x3 rotation = Matrix3x3::rotate(obj.rotation);

    // Rotate camera after translating
    // The transpose of a rotation is equal to the inverse
    Matrix4x4 view = Matrix4x4::rotate(Camera::main->rotation).transpose()
        * Matrix4x4::translate(-Camera::main->position);

    if (Camera::main->mode == Camera::PERSPECTIVE)
    {
        if (obj.dirty & Object::TRANSFORM)
        {
            obj.dirty &= ~Object::TRANSFORM;
            Vector3 min = obj.mesh->aabbMin;
            Vector3 max = obj.mesh->aabbMax;
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
                obbPoints[i] = rotation*(obbPoints[i] * obj.scale);
                for (int j = 0; j < 3; ++j)
                {
                    min[j] = std::min(min[j], obbPoints[i][j]);
                    max[j] = std::max(max[j], obbPoints[i][j]);
                }
            }
            obj.minBounds = min;
            obj.maxBounds = max;
        }
        Vector3 min = obj.minBounds + obj.position;
        Vector3 max = obj.maxBounds + obj.position;
        
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
    
    Material &mat = *obj.material;
    Shader &shader = *mat.shader;
    Texture *tex = mat.mainTex;

    Matrix4x4 rotation4x4 = add_r_c(rotation);
   
    Matrix4x4 model =
        Matrix4x4::translate(obj.position)
        * rotation4x4
        * Matrix4x4::scale(obj.scale);

    Matrix4x4 mvp = Camera::main->projection * view * model;

    obj.mesh->varray->bind();
    shader.bind();
    Renderer::bind_material(obj.material);
    shader.set_uniform_mat4x4("u_mvp", mvp);
    shader.set_uniform_mat4x4("u_model", model);
    shader.set_uniform_mat4x4("u_rotation", rotation4x4);
    shader.set_uniform3f("u_camPos", Camera::main->position);
    if (tex)
    {
        tex->bind();
        shader.set_uniform1i("u_tex", tex->get_slot());
    }
    GLCall(glDrawElements(GL_TRIANGLES, obj.mesh->tris.size(), GL_UNSIGNED_INT, nullptr));
}
static bool test_aabb_1D(float min0, float max0, float min1, float max1)
{
    return min1<max0 && min0<max1;
}
void Renderer::draw(Object2D &obj)
{
    if (!obj.enabled)
        return;
    Matrix2x2 rotation = Matrix2x2::rotate(obj.rotation);
    {
        if (obj.dirty & Object2D::TRANSFORM)
        {
            obj.dirty &= ~Object2D::TRANSFORM;
            Vector2 min = obj.mesh->aabbMin;
            Vector2 max = obj.mesh->aabbMax;
            Vector2 obbPoints[4] = {
                Vector2(min.x, min.y),
                Vector2(max.x, min.y),
                Vector2(min.x, max.y),
                Vector2(max.x, max.y)
            };
            min = Vector3(Math::INF, Math::INF, Math::INF);
            max = Vector3(-Math::INF, -Math::INF, -Math::INF);
            for (int i = 0; i < 4; ++i)
            {
                obbPoints[i] = rotation*(obbPoints[i] * obj.scale);
                for (int j = 0; j < 2; ++j)
                {
                    min[j] = std::min(min[j], obbPoints[i][j]);
                    max[j] = std::max(max[j], obbPoints[i][j]);
                }
            }
            obj.minBounds = min;
            obj.maxBounds = max;
        }
        Vector2 camPos = Camera::main->position;
        Vector2 camScale = Renderer::aspectRatio*Camera::main->orthoSize;
        Vector2 camMin = camPos-camScale;
        Vector2 camMax = camPos+camScale;
        Vector2 objMin = obj.minBounds + obj.position;
        Vector2 objMax = obj.maxBounds + obj.position;
        if (!(test_aabb_1D(camMin.x, camMax.x, objMin.x, objMax.x)
            && test_aabb_1D(camMin.y, camMax.y, objMin.y, objMax.y)))
        {
            obj.m_wasCulled = 1;
            return;
        }
        obj.m_wasCulled = 0;
    }

    Matrix4x4 rotation4x4 = Matrix4x4(
        rotation[0][0], rotation[0][1], 0, 0,
        rotation[1][0], rotation[1][1], 0, 0,
        0,              0,              1, 0,
        0,              0,              0, 1
    );

    // Rotate camera after translating
    // The transpose of a rotation is equal to the inverse
    Matrix4x4 view = Matrix4x4::rotate(Camera::main->rotation).transpose()
        * Matrix4x4::translate(-Camera::main->position);

    Material &mat = *obj.material;
    Shader &shader = *mat.shader;
    Texture *tex = mat.mainTex;
   
    Matrix4x4 model =
        Matrix4x4::translate(obj.position)
        * rotation4x4
        * Matrix4x4::scale(obj.scale);

    Matrix4x4 mvp = Camera::main->projection * view * model;

    obj.mesh->varray->bind();
    shader.bind();
    Renderer::bind_material(obj.material);
    shader.set_uniform_mat4x4("u_mvp", mvp);
    shader.set_uniform_mat4x4("u_model", model);
    shader.set_uniform_mat4x4("u_rotation", rotation4x4);
    shader.set_uniform3f("u_camPos", Camera::main->position);
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

    Vector2 world_pos = img.calc_world_pos();
    
    // TRS
    Matrix2x2 rotation = Matrix2x2::rotate(img.rotation);
    Matrix4x4 rotation4x4 = Matrix4x4(
        rotation[0][0], rotation[0][1], 0, 0,
        rotation[1][0], rotation[1][1], 0, 0,
        0,              0,              1, 0,
        0,              0,              0, 1
    );
    Matrix4x4 proj = Matrix4x4(
        reverseAspect.x, 0,               0,               0,
        0,               reverseAspect.y, 0,               0,
        0,               0,               1,               0,
        0,               0,               0,               1
    );
    // Projection is already applied to translation, so translate after projecting
    Matrix4x4 model = Matrix4x4::translate(world_pos.x, world_pos.y, 0)
        * proj * rotation4x4 * Matrix4x4::scale(img.scale.x, img.scale.y, 1);
    //Matrix4x4 model = Matrix4x4(
    //    scale.x, 0,       0,       world_pos.x,
    //    0,       scale.y, 0,       world_pos.y,
    //    0,       0,       1,       0,
    //    0,       0,       0,       1
    //);
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
    text.mesh->varray->bind();
    UIText::shader()->bind();
    float charSize = Math::min(text.scale.x/text.str.size()*2.5f, text.scale.y);
    float textWidth = charSize*text.str.size()/2.5f;

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
    if (fireEvents)
    {
        preRender->fire();
        Physics::simulate(Time::delta());
        
        postSimulation->fire();
    }

    if (Camera::main->mode == Camera::PERSPECTIVE)
    {
        GLCall(glEnable(GL_DEPTH_TEST));
        GLCall(glDisable(GL_BLEND));
    }
    if (Object2D::sortObjects)
    {
        Object2D::sortObjects = 0;
        auto &objs = Object2D::all;
        auto predicate = [](Object2D *a, Object2D *b)
        { return a->zIndex() < b->zIndex(); };

        for (unsigned int i = 1; i < objs.size(); ++i)
        {
            if (predicate(objs[i], objs[i-1]))
            {
                Object2D *element = objs[i];
                int j = i-1;
                while (j >= 0 && predicate(element, objs[j]))
                {
                    objs[j+1] = objs[j];
                    objs[j+1]->objectIndex = j+1;
                    --j;
                }
                objs[j+1] = element;
                objs[j+1]->objectIndex = j+1;
            }
        }
    }
    clear();
    for (Object *obj : Object::allObjects) draw(*obj);
    for (Object2D *obj : Object2D::all) draw(*obj);
/*
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)); // wireframe
*/
    GLCall(glClear(GL_DEPTH_BUFFER_BIT));
    if (Camera::main->mode == Camera::PERSPECTIVE)
    {
        GLCall(glDisable(GL_DEPTH_TEST));
        GLCall(glEnable(GL_BLEND));
    }
    
    auto &uiObjs = UIObject::uiObjects;
    if (UIObject::sortObjects)
    {
        Math::insertion_sort<UIObject>(*uiObjs, [](UIObject &a, UIObject &b)
        { return a.render_order() < b.render_order(); });

        for (unsigned int i = 0; i < uiObjs->size(); ++i)
            (*uiObjs)[i].renderer_id() = i;
        
        UIObject::sortObjects = 0;
    }
    for (auto &ui : *uiObjs)
    {
        if (ui.type == UIObject::UIIMAGE) draw(*((UIImage*)ui.data));
        else if (ui.type == UIObject::UITEXT) draw(*((UIText*)ui.data));
    }
    if (fireEvents) postRender->fire();
}
Signal &Renderer::aspect_ratio_changed() { return  aspectChanged->signal; }
Signal &Renderer::pre_render() { return preRender->signal; }
Signal &Renderer::post_simulation() { return postSimulation->signal; }
Signal &Renderer::post_render() { return postRender->signal; }