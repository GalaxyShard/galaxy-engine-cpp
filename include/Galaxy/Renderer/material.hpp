#pragma once
#include <unordered_map>
#include <vector>
#include <Galaxy/event.hpp>
#include <Galaxy/Math/matrix.hpp>
class Shader;
class Material;
class Texture;

//class MatComponent
//{
//private:
//    friend class Material;
//    friend class Renderer;
//public:
//    Material* material();
//    ~MatComponent();

//protected:
//    Material *m_material = 0;
//    virtual void pre_render() {}
//};

struct Uniform
{
    enum Type { FLOAT, VEC2, VEC3, VEC4, INT, MAT4x4 };
    union Value
    {
        float v;
        Vector2 v2;
        Vector3 v3;
        Vector4 v4;
        int i;
        Matrix4x4 m4x4;
    } value;
    Type type;
    
    Uniform(const float &v);
    Uniform(const Vector2 &v);
    Uniform(const Vector3 &v);
    Uniform(const Vector4 &v);
    Uniform(const int &v);
    Uniform(const Matrix4x4 &v);
};
class Material
{
private:
    //std::vector<MatComponent*> components;

    std::unordered_map<int, Uniform> uniforms;
    friend class Renderer;
    //friend class MatComponent;
public:
    Material(Shader *shader);
    //~Material();

    Texture *mainTex = 0;
    Shader *shader;
    void set_uniform(const char *uniform, Uniform value);

    //template<typename T>
    //T* add_component();
};


//template<typename T>
//T* Material::add_component()
//{
//    static_assert(std::is_base_of<MatComponent, T>::value, "T must inherit MatComponent");
//    T *comp = new T();
//    comp->m_material = this;
//    components.push_back(comp);
//    return comp;
//}