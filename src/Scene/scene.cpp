#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/object.hpp>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/text.hpp>
#include <Galaxy/UI/group.hpp>
#include <Galaxy/Math/binary.hpp>
#include <Events/frameQueuer.hpp>

Scene *Scene::activeScene=0;

struct SceneInfo
{
    Event onInit;
};
std::map<std::string, SceneInfo> Scene::sceneEvents;
void Scene::initialize(std::string name)
{
    this->name = name;
    activeScene = this;
    if (sceneEvents.count(name))
    {
        SceneInfo &info = sceneEvents[name];
        info.onInit.fire();
    }
}
Scene* Scene::create(std::string name)
{
    Scene *scene = new Scene();
    // Dont initialize immediately to sync with the destroy() function
    //scene->initialize(name);

    char *data = (char*)malloc(sizeof(Scene*)+name.size()+1);
    memcpy(data, &scene, sizeof(Scene*));
    // c_str is null-terminated, size() is without null terminator
    memcpy(data+sizeof(Scene*), name.c_str(), name.size()+1);
    FrameQueuer::queue_next_frame([data]()
    {
        b_cast<Scene*>(data)->initialize(std::string(data+sizeof(Scene*)));
        free(data);
    });
    return scene;
}
void Scene::destroy(Scene *scene)
{
    FrameQueuer::queue_next_frame([scene]() { delete scene; });
    //delete scene;
    //FrameQueuer::queue_next_frame(scene, FrameQueuer::DELSCENE);
}
Scene::~Scene()
{
    for (auto &[id, comp] : components)
        comp.destructor(comp.data);
    destroyingScene = 1;
    
    for (auto &v : instances)
    {
        if (v.type==OBJ) delete (Object*)v.ptr;
        else if (v.type==OBJ2D) delete (Object2D*)v.ptr;
        else if (v.type==IMG) delete (UIImage*)v.ptr;
        else if (v.type==TXT) delete (UIText*)v.ptr;
        else if (v.type==GROUP) delete (UIGroup*)v.ptr;
        else assert(false);
    }
}
unsigned int Scene::add_inst(void *inst, Scene::InstType type)
{
    instances.emplace_back(inst,type);
    return instances.size()-1;
}
void Scene::remove_inst(unsigned int id)
{
    if (destroyingScene) return;

    if (id==instances.size()-1)
    {
        instances.pop_back();
        return;
    }
    instances[id] = std::move(instances.back());
    auto &v = instances[id];

    if (v.type==OBJ) ((Object*)v.ptr)->sceneID = id;
    else if (v.type==OBJ2D) ((Object2D*)v.ptr)->sceneID = id;
    else if (v.type==IMG) ((UIImage*)v.ptr)->sceneID = id;
    else if (v.type==TXT) ((UIText*)v.ptr)->sceneID = id;
    else if (v.type==GROUP) ((UIGroup*)v.ptr)->sceneID = id;
    else assert(false);
    instances.pop_back();
}
void Scene::on_init(std::string name, void(*func)())
{
    sceneEvents[name].onInit.signal.connect_int(func);
}