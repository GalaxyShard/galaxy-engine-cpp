#pragma once
#include <Galaxy/Math/vector.hpp>
#include <vector>
#include <unordered_map>
class Material;
class Mesh;
class Scene;
class Shader;
class Texture;
struct Uniform;

struct TransformECS
{
    Vector3 i_pos;
    Vector3 i_scale = Vector3(1,1,1);
    Vector3 i_rotation;

    TransformECS(){}
    TransformECS(Vector3 pos,Vector3 scale,Vector3 rotation)
        :i_pos(pos),i_scale(scale),i_rotation(rotation){}
    inline Vector3 const& pos() { return i_pos; }
    inline Vector3 const& scale() { return i_scale; }
    inline Vector3 const& rotation() { return i_rotation; }
};

class Object;
class ObjComponent
{
private:

public:
    Object *obj = 0;
    virtual ~ObjComponent() = default;
};

class Object
{
private:
    static std::unique_ptr<std::vector<Object*>> allObjects;
    static bool sortObjects;
    unsigned int objectIndex;
    int renderOrder = 0;

    Scene *scene=0;
    friend class Renderer;
public:
    std::vector<std::unique_ptr<ObjComponent>> components;

    Mesh *mesh=0;
    Material *material=0;

    Vector3 position, scale = Vector3(1, 1, 1);
    Vector3 rotation;

    void render_order(int order) { renderOrder = order; }
    int render_order() { return renderOrder; }
        
    Object(Mesh *mesh, Material *mat, bool isStatic = false);
    ~Object();

    Object(const Object&) = delete;
    void operator=(const Object&) = delete;

    template<typename T>
    T* add_component()
    {
        static_assert(std::is_base_of<ObjComponent, T>::value, "Class must be an ObjComponent");
        components.push_back(std::make_unique<T>());
        T *comp = (T*)components[components.size()-1].get();
        comp->obj = this;
        return comp;
    }
    template<typename T>
    T* get_component()
    {
        static_assert(std::is_base_of<ObjComponent, T>::value, "Class must be an ObjComponent");
        for (int i = 0; i < components.size(); ++i)
        {
            if (T *component = dynamic_cast<T*>(components[i].get()))
                return component;
        }
        return 0;
    }
};