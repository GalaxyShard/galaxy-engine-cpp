#pragma once
#include <Galaxy/Math/vector.hpp>
#include <vector>
class Material;
class Mesh;
class Scene;

class Object
{
    private:
        static std::unique_ptr<std::vector<Object*>> allObjects;
        static bool sortObjects;
        unsigned int objectIndex;
        int renderOrder = 0;

        Scene *scene;
        friend class Renderer;
    public:
        Mesh *mesh;
        Material *material;

        Vector3 position, scale = Vector3(1, 1, 1);
        Quaternion rotation = Quaternion::identity();

        void render_order(int order) { renderOrder = order; }
        int render_order() { return renderOrder; }
        
        Object(Mesh *mesh, Material *mat, bool isStatic = false);
        ~Object();

        Object(const Object&) = delete;
        void operator=(const Object&) = delete;
};