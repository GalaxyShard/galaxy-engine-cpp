#pragma once
#include <Galaxy/Math/vector.hpp>
#include <vector>
#include <unordered_map>
#include <Galaxy/types.hpp>
class Material;
class Mesh;
class Scene;
class Shader;
class Texture;
struct Uniform;

class Object;
// ObjComponent OUTDATED
class ObjComponent
{
private:

public:
    Object *obj = 0;
    virtual ~ObjComponent(){}
};

class Object
{
private:
    static std::vector<Object*> allObjects;
    Scene *scene=0;
public:
    std::vector<std::unique_ptr<ObjComponent>> components;
    Mesh *mesh=0;
    Material *material=0;

    Vector3 position, scale = Vector3(1, 1, 1);
    Vector3 rotation;
private:
    Vector3 minBounds, maxBounds;
    unsigned int objectIndex, sceneID;
    ucharG dirty = 0xff;

    Object() = default;

    friend class Renderer;
    friend class Scene;
public:
    bool enabled = 1;
    // Call when scale/rotation changes
    void dirtyBounds() { dirty |= 1; }
    
    static Object* create(Mesh *mesh, Material *mat);
    static void destroy(Object *obj);
    ~Object();

    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = delete;
    Object& operator=(Object&&) = delete;

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
        for (unsigned int i = 0; i < components.size(); ++i)
        {
            if (T *component = dynamic_cast<T*>(components[i].get()))
                return component;
        }
        return 0;
    }
};