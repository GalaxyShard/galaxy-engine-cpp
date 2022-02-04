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
Scene::Scene(std::string name) : name(name)
{
    activeScene = this;
    if (sceneEvents.count(name))
    {
        SceneInfo &info = sceneEvents[name];
        if (info.createCallback) info.createCallback();

        info.onInit.fire();
    }
}
template<typename T>
void delete_vector(std::vector<T*> &vector)
{
    while (vector.size() != 0)
    {
        delete vector[vector.size()-1];
        vector.pop_back();
    }
}
Scene::~Scene()
{
    for (auto &[id, comp] : components)
    {
        comp.destructor(comp.data);
    }
    components.clear();
    destroyingScene = 1;
    delete_vector(objInstances);
    delete_vector(imgInstances);
    delete_vector(textInstances);
    delete_vector(groupInstances);

    sceneEvents[name].onDestroy.fire();
}
template<typename T>
static void remove(T *data, std::vector<T*> &instances)
{
    int instID = -1;
    for (int i = 0; i < instances.size(); ++i)
    {
        if (instances[i] == data)
        {
            instID = i;
            break;
        }
    }
    assert(instID != -1);
    std::swap(instances[instID], instances.back());
    instances.pop_back();
}
void Scene::remove_inst(Object *data)
{
    if (destroyingScene) return;
    remove(data, objInstances);
}
void Scene::remove_inst(UIImage *data)
{
    if (destroyingScene) return;
    remove(data, imgInstances);
}
void Scene::remove_inst(UIText *data)
{
    if (destroyingScene) return;
    remove(data, textInstances);
}
void Scene::remove_inst(UIGroup *data)
{
    if (destroyingScene) return;
    remove(data, groupInstances);
}
void Scene::on_init(std::string name, void(*func)())
{
    sceneEvents[name].onInit.signal->connect_int(func);
}
void Scene::set_create_callback(std::string name, void(*func)())
{
    sceneEvents[name].createCallback = func;
}
void Scene::on_destroy(std::string name, void(*func)())
{
    sceneEvents[name].onDestroy.signal->connect_int(func);
}