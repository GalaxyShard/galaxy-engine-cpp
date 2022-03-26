#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/object.hpp>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/group.hpp>

Scene *Scene::activeScene;

struct SceneInfo
{
    void(*createCallback)();
    Event onInit;
    Event onDestroy;
};
std::unordered_map<std::string, SceneInfo> Scene::sceneEvents;
void Scene::initialize(std::string name)
{
    this->name = name;
    activeScene = this;
    if (sceneEvents.count(name))
    {
        SceneInfo &info = sceneEvents[name];
        if (info.createCallback) info.createCallback();

        info.onInit.fire();
    }
}
Scene* Scene::create(std::string name)
{
    Scene *scene = new Scene();
    scene->initialize(name);
    return scene;
}
//Scene::Scene(std::string name)
//{
//    initialize(name);
//}
//template<typename T>
//void delete_vector(std::vector<T*> &vector)
//{
//    for (T *element : vector) delete element;
//    //while (vector.size() != 0)
//    //{
//    //    delete vector[vector.size()-1];
//    //    vector.pop_back();
//    //}
//}
Scene::~Scene()
{
    destroyingScene = 1;
    for (auto &[id, comp] : components)
        comp.destructor(comp.data);
    
    //components.clear();
    for (auto &v : instances)
    {
        if (v.type==OBJ) delete (Object*)v.ptr;
        else if (v.type==IMG) delete (UIImage*)v.ptr;
        else if (v.type==TXT) delete (UIText*)v.ptr;
        else if (v.type==GROUP) delete (UIGroup*)v.ptr;
        else assert(false);
    }
    //delete_vector(objInstances);
    //delete_vector(imgInstances);
    //delete_vector(textInstances);
    //delete_vector(groupInstances);

    sceneEvents[name].onDestroy.fire();
}
unsigned int Scene::add_inst(void *inst, Scene::InstType type)
{
    instances.emplace_back(inst,type);
    return instances.size()-1;
}
void Scene::remove_inst(unsigned int id)
{
    if (destroyingScene) return;

    auto &v = instances.back();
    std::swap(v, instances[id]);
    instances.pop_back();
    if (v.type==OBJ) ((Object*)v.ptr)->sceneID = id;
    else if (v.type==IMG) ((UIImage*)v.ptr)->sceneID = id;
    else if (v.type==TXT) ((UIText*)v.ptr)->sceneID = id;
    else if (v.type==GROUP) ((UIGroup*)v.ptr)->sceneID = id;
    else assert(false);
}
//template<typename T>
//static void remove(T *data, std::vector<T*> &instances)
//{
//    unsigned int instID = ~0U;
//    for (unsigned int i = 0; i < instances.size(); ++i)
//    {
//        if (instances[i] == data)
//        {
//            instID = i;
//            break;
//        }
//    }
//    assert(instID != ~0U && "Instance not found");
//    std::swap(instances[instID], instances.back());
//    instances.pop_back();
//}
//void Scene::remove_inst(Object *data)
//{
//    if (destroyingScene) return;
//    remove(data, objInstances);
//}
//void Scene::remove_inst(UIImage *data)
//{
//    if (destroyingScene) return;
//    remove(data, imgInstances);
//}
//void Scene::remove_inst(UIText *data)
//{
//    if (destroyingScene) return;
//    remove(data, textInstances);
//}
//void Scene::remove_inst(UIGroup *data)
//{
//    if (destroyingScene) return;
//    remove(data, groupInstances);
//}
void Scene::on_init(std::string name, void(*func)())
{
    sceneEvents[name].onInit.signal.connect_int(func);
}
void Scene::set_create_callback(std::string name, void(*func)())
{
    sceneEvents[name].createCallback = func;
}
void Scene::on_destroy(std::string name, void(*func)())
{
    sceneEvents[name].onDestroy.signal.connect_int(func);
}