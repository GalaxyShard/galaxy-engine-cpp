#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/object.hpp>
#include <Galaxy/init.hpp>
#include <iostream>

#include <gldebug.hpp>
auto Object::allObjects = std::vector<Object*>();

//Object::Object(Mesh *mesh, Material *mat, bool isStatic) : mesh(mesh), material(mat)
//{
//    allObjects.push_back(this);
//    objectIndex = allObjects.size() - 1;
//
//    scene = Scene::activeScene;
//    if (scene)
//        scene->objInstances.push_back(this);
//}
Object* Object::create(Mesh *mesh, Material *mat)
{
    Object *obj = new Object();
    obj->mesh = mesh;
    obj->material = mat;
    allObjects.push_back(obj);

    obj->objectIndex = allObjects.size() - 1;
    obj->scene = Scene::activeScene;
    if (obj->scene)
        obj->sceneID = obj->scene->add_inst(obj, Scene::OBJ);
        //obj->scene->objInstances.push_back(obj);

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
    std::swap(allObjects[objectIndex], allObjects.back());
    allObjects.pop_back();
    obj2->objectIndex = objectIndex;
}