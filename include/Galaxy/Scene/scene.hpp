#pragma once
#include <vector>
#include <unordered_map>
#include <string>
class Object;
class UIImage;
class UIText;
class UIGroup;
class Mesh;
class Texture;
class Material;
class Event;

class Scene
{
private:
    std::string name;
    bool destroyingScene = 0;
    
    std::vector<Object*> objInstances;
    std::vector<UIImage*> imgInstances;
    std::vector<UIText*> textInstances;
    std::vector<UIGroup*> groupInstances;

    static std::unordered_map<std::string, std::pair<void(*)(), Event>> sceneEvents;
    
    void remove_inst(Object *data);
    void remove_inst(UIImage *data);
    void remove_inst(UIText *data);
    void remove_inst(UIGroup *data);

    friend class UIImage;
    friend class UIText;
    friend class Object;
    friend class UIGroup;
public:
    // all created objects (NOT assets) are added to the active scene
    static Scene *activeScene;

    static Scene* parse(const std::string &path, std::string name);
    inline const std::string& get_name() { return name; }

    /* Fired right after create_callback, for using the scene objects */
    static void on_init(std::string name, void(*func)());
    /* Sets up the scene, creates all of the proper UI, etc */
    static void set_create_callback(std::string name, void(*func)());

    Scene(std::string name);
    ~Scene();
    Scene(const Scene&) = delete;
    void operator=(const Scene&) = delete;
};