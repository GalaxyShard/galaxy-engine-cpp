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

    enum DirtyFlags : uint8_t { TRANSFORM=1<<0 };
    uint8_t dirty = 0xff;

    Object() = default;

    friend class Renderer;
    friend class Scene;
public:
    bool enabled = 1;
    // Call when scale/rotation changes
    void dirtyBounds() { dirty |= TRANSFORM; }
    
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
class Object2D
{
private:
    static std::vector<Object2D*> all;
    static bool sortObjects;
    Scene *scene=0;
public:
    std::vector<std::unique_ptr<ObjComponent>> components;
    Mesh *mesh=0;
    Material *material=0;

    Vector2 position, scale = Vector2(1, 1);
    float rotation = 0;
private:
    float m_zIndex = 0;
    Vector2 minBounds, maxBounds;
    unsigned int objectIndex, sceneID;

    enum DirtyFlags : uint8_t { TRANSFORM=1<<0 };
    uint8_t dirty = 0xff;
    bool m_wasCulled = 0;

    Object2D() = default;

    friend class Renderer;
    friend class Scene;
public:
    bool enabled = 1;
    // Call when scale/rotation changes
    void dirtyBounds() { dirty |= TRANSFORM; }
    float zIndex() { return m_zIndex; }
    void zIndex(float z) { m_zIndex = z; }
    bool wasCulled() { return m_wasCulled; }
    
    static Object2D* create(Mesh *mesh, Material *mat);
    static void destroy(Object2D *obj);
    ~Object2D();

    Object2D(const Object2D&) = delete;
    Object2D& operator=(const Object2D&) = delete;
    Object2D(Object2D&&) = delete;
    Object2D& operator=(Object2D&&) = delete;
};