#include <Galaxy/Renderer/material.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Renderer/texture.hpp>

#include <gldebug.hpp>

Material::Material(Shader *shader) : shader(shader) { }
void Material::set_uniform(const char *uniform, Uniform value)
{
    int location = shader->u_location(uniform);
    if (location != -1)
    {
        uniforms[location].type = value.type;
        uniforms[location].value = value.value;
    }
}
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
