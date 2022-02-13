#pragma once
#include <unordered_map>
#include <vector>
#include <Galaxy/Events/event.hpp>
#include <Galaxy/Math/matrix.hpp>
class Shader;
class Material;
class Texture;

struct Uniform
{
    enum Type { FLOAT, VEC2, VEC3, VEC4, INT, MAT4x4 };
    Type type;
    union Value
    {
        float v;
        Vector2 v2;
        Vector3 v3;
        Vector4 v4;
        int i;
        Matrix4x4 m4x4;
        Value() { memset(this, 0, sizeof(Value)); }
        Value(float val) { v = val; }
        Value(Vector2 val) { v2 = val; }
        Value(Vector3 val) { v3 = val; }
        Value(Vector4 val) { v4 = val; }
        Value(int val) { i = val; }
        Value(Matrix4x4 val) { m4x4 = val; }
    } value;
    
    Uniform(const Uniform&);
    Uniform(Uniform&&);
    Uniform& operator=(const Uniform&);
    Uniform& operator=(Uniform&&);

    Uniform();
    Uniform(const float &);
    Uniform(const Vector2 &);
    Uniform(const Vector3 &);
    Uniform(const Vector4 &);
    Uniform(const int &);
    Uniform(const Matrix4x4 &);
};
class Material
{
private:
    std::unordered_map<int, Uniform> uniforms;
    friend class Renderer;
public:
    Material(Shader *shader);

    Texture *mainTex = 0;
    Shader *shader;
    void set_uniform(const char *uniform, Uniform value);
};