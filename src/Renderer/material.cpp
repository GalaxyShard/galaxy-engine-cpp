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
    //sizeof(Uniform::Value);
    int location = shader->u_location(uniform);
    if (location != -1)
    {
        uniforms[location].type = value.type;
        uniforms[location].value = value.value;
    }
    //if (location != -1) uniforms.emplace(location, value);
    //if (location != -1) uniforms.insert(std::make_pair(location, value));
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

static void copy_val(Uniform &dest, const Uniform &src)
{
    if (src.type == Uniform::FLOAT) dest.value.v = src.value.v;
    else if (src.type == Uniform::VEC2) dest.value.v2 = src.value.v2;
    else if (src.type == Uniform::VEC3) dest.value.v3 = src.value.v3;
    else if (src.type == Uniform::VEC4) dest.value.v4 = src.value.v4;
    else if (src.type == Uniform::INT) dest.value.i = src.value.i;
    else if (src.type == Uniform::MAT4x4) dest.value.m4x4 = src.value.m4x4;

}
Uniform& Uniform::operator=(const Uniform &uniform)
{
    //value = v.value;
    copy_val(*this, uniform);
    type = uniform.type;
    return *this;
}
Uniform& Uniform::operator=(Uniform &&uniform)
{
    copy_val(*this, uniform);
    type = uniform.type;
    return *this;
}
Uniform::Uniform() {  }
Uniform::Uniform(const float &val) : type(FLOAT) { value = val; }
Uniform::Uniform(const Vector2 &val) : type(VEC2) { value = val; }
Uniform::Uniform(const Vector3 &val) : type(VEC3) { value = val; }
Uniform::Uniform(const Vector4 &val) : type(VEC4) { value = val; }
Uniform::Uniform(const int &val) : type(INT) { value = val; }
Uniform::Uniform(const Matrix4x4 &val) : type(MAT4x4) { value = val; }

Uniform::Uniform(const Uniform &uniform) : type(uniform.type) { copy_val(*this, uniform); }
Uniform::Uniform(Uniform &&uniform) : type(uniform.type) { copy_val(*this, uniform); }

//Uniform::Uniform(const float &v) : value({.v = v}), type(FLOAT) { }
//#define DEFINE_CTOR(t0, t1, var) Uniform::Uniform(const t0 &v) : type(t1) { value.var = v; }
//#define DEFINE_CTOR(t0, t1, var) Uniform::Uniform(const t0 &val) : type(t1) { value.var = val; }
//#define DEFINE_CTOR(t0, t1, var) Uniform::Uniform(const t0 &v) : value({.var = v}), type(t1) { }

//DEFINE_CTOR(float, FLOAT, v);
//DEFINE_CTOR(Vector2, VEC2, v2);
//DEFINE_CTOR(Vector3, VEC3, v3);
//DEFINE_CTOR(Vector4, VEC4, v4);
//DEFINE_CTOR(int, INT, i);
//DEFINE_CTOR(Matrix4x4, MAT4x4, m4x4);
//#undef DEFINE_CTOR