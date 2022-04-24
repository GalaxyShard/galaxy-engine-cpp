#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/object.hpp>
#include <Galaxy/init.hpp>
#include <iostream>

#include <gldebug.hpp>
auto Object::allObjects = std::vector<Object*>();

Object* Object::create(Mesh *mesh, Material *mat)
{
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



auto Object2D::all = std::vector<Object2D*>();
bool Object2D::sortObjects = 0;

Object2D* Object2D::create(Mesh *mesh, Material *mat)
{
    Object2D *obj = new Object2D();
    obj->mesh = mesh;
    obj->material = mat;
    obj->objectIndex = all.size();
    all.push_back(obj);

    obj->scene = Scene::activeScene;
    if (obj->scene)
        obj->sceneID = obj->scene->add_inst(obj, Scene::OBJ2D);

    sortObjects = 1;
    return obj;
}
void Object2D::destroy(Object2D *obj)
{
    delete obj;
}
Object2D::~Object2D()
{
    scene->remove_inst(sceneID);

    Object2D *obj2 = all.back();
    if (obj2 == this)
    {
        all.pop_back();
        return;
    }
    all[objectIndex] = std::move(all.back());
    all.pop_back();
    obj2->objectIndex = objectIndex;
    sortObjects = 1;
}