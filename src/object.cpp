#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/object.hpp>
#include <Galaxy/init.hpp>
#include <iostream>

#include <gldebug.hpp>
auto Object::allObjects = std::vector<Object*>();

Object* Object::create(Mesh *mesh, Material *mat)
{
    assert(mesh != nullptr && mat != nullptr);
    Object *obj = new Object();
    obj->mesh = mesh;
    obj->material = mat;
    obj->objectIndex = allObjects.size();
    allObjects.push_back(obj);

    obj->scene = Scene::activeScene;
    if (obj->scene)
        obj->sceneID = obj->scene->add_inst(obj, Scene::OBJ);

    return obj;
}
void Object::destroy(Object *obj)
{
    delete obj;
}
Object::~Object()
{
    scene->remove_inst(sceneID);

    Object *obj2 = allObjects.back();
    if (obj2 == this)
    {
        allObjects.pop_back();
        return;
    }
    allObjects[objectIndex] = std::move(allObjects.back());
    allObjects.pop_back();
    obj2->objectIndex = objectIndex;
}