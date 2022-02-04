#pragma once
#include <Galaxy/Math/vector.hpp>
#include <vector>
class Material;
class Mesh;
class Scene;

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