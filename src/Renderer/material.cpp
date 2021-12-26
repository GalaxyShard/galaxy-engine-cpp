#include <Galaxy/Renderer/material.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <debug.hpp>

Material::Material(Shader *shader) : shader(shader) { }
//Material::~Material()
//{
//    for (auto &v : components) delete v;
//}
void Material::set_uniform(const char *uniform, Uniform value)
{
    int location = shader->u_location(uniform);
    if (location != -1) uniforms.insert(std::make_pair(location, value));
}
//void Material::add_component(MatComponent component)
//{
//    // possibly use templates with add_component to prevent manual creation, similar to Unity
//    //if (component.m_material) assert("Same component used on multiple materials");
    
//    component.m_material = this;
//    components.push_back(component);
//}
//Material* MatComponent::material() { return m_material; }
//MatComponent::~MatComponent()
//{
//    if (!m_material) return;
//    auto &c = m_material->components;
//    c.erase(std::find(c.begin(), c.end(), this));
//}


#define DEFINE_CTOR(t0, t1, var) Uniform::Uniform(const t0 &v) : value({.var = v}), type(t1) { }
DEFINE_CTOR(float, FLOAT, v);
DEFINE_CTOR(Vector2, VEC2, v2);
DEFINE_CTOR(Vector3, VEC3, v3);
DEFINE_CTOR(Vector4, VEC4, v4);
DEFINE_CTOR(int, INT, i);
DEFINE_CTOR(Matrix4x4, MAT4x4, m4x4);
#undef DEFINE_CTOR