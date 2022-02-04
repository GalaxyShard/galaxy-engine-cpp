#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/object.hpp>
#include <Galaxy/init.hpp>

#include <iostream>
#include <debug.hpp>
auto Object::allObjects = std::make_unique<std::vector<Object*>>();
bool Object::sortObjects = 0;

Object::Object(Mesh *mesh, Material *mat, bool isStatic) : mesh(mesh), material(mat)
{
    allObjects->push_back(this);
    objectIndex = allObjects->size() - 1;
    sortObjects = 1;

    scene = Scene::activeScene;
    if (scene)
        scene->objInstances.push_back(this);
}
Object::~Object()
{
    scene->remove_inst(this);

    Object *obj2 = allObjects->back();
    std::swap((*allObjects)[objectIndex], allObjects->back());
    obj2->objectIndex = objectIndex;
    allObjects->pop_back();
    
    sortObjects = 1;
}