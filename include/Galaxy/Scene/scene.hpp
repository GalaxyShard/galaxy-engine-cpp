#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <typeindex>
class Object;
class UIImage;
class UIText;
class UIGroup;
class Mesh;
class Texture;
class Material;
class Event;

#define ADD_SCENE_COMPONENT(sceneName, comp) \
    static void comp##_init() { \
        Scene::on_init(sceneName, []() { Scene::activeScene->add_component<comp>(); }); \
    } \
    FIRST_INIT_FUNC(comp##_init)

struct SceneInfo;
struct SceneComponent
{
    void *data;
    void (*destructor)(void *);
};
class Scene
{
private:
    enum InstType : uintG { OBJ,IMG,TXT,GROUP };
    struct SceneInst
    {
        void *ptr;
        InstType type;
        SceneInst(void *p, InstType t):ptr(p),type(t){}
    };
    static std::map<std::string, SceneInfo> sceneEvents;

    std::map<std::type_index, SceneComponent> components;
    std::vector<SceneInst> instances;
    std::string name;
    bool destroyingScene = 0;

    void initialize(std::string name);
    unsigned int add_inst(void *inst, InstType type);
    void remove_inst(unsigned int id);

    Scene() = default;
    
    friend class UIImage;
    friend class UIText;
    friend class Object;
    friend class UIGroup;
public:
    // all created objects (NOT assets) are added to the active scene
    static Scene *activeScene;

    static Scene* parse(const std::string &path, std::string name);

    static Scene* create(std::string name);
    static void destroy(Scene *scene);

    /* Fired right after create_callback, for using the scene objects */
    static void on_init(std::string name, void(*func)());
    /* Sets up the scene, creates all of the proper UI, etc */
    //static void set_create_callback(std::string name, void(*func)());
    /* Fired when the destructor is called */
    //static void on_destroy(std::string name, void(*func)());

    ~Scene();
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;

    inline const std::string& get_name() { return name; }

    template<typename T>
    T* get_component()
    {
        return (T*)components[std::type_index(typeid(T))].data;
    }
    template<typename T>
    void add_component()
    {
        auto type = std::type_index(typeid(T));
        if (components.count(type))
            assert(false && "Already added component to scene");
        components[type].data = new T();
        components[type].destructor = [](void *data)
        {
            delete (T*)data;
        };
    }
};