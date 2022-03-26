#include <Galaxy/Scene/scene.hpp>
#include <Galaxy/Math/binary.hpp>
#include <fstream>

/*
    .gscene File Format
    All stored in big endian

    Asset info:
        uintG length;
        (string path, assetType)[] assets;
    UI info:
        uintG imageCount, textCount;
        UIImage[] images;
        UIText[] texts;
*/
#define STR1(x) #x
#define STR0(x) STR1(x) 
#define STR(x) STR0(x)
#define READ_INT(var, errorMsg) if (!stream.read(buffer, 4)) throw(errorMsg); var = to_native_endian32(buffer, Endian::BIG);
#define ERRMSG(msg) (STR(__FILE__) ":" STR(__LINE__) " " msg)
static void parse_scene(Scene *scene, const std::string &path)
{
    std::ifstream stream = std::ifstream(path, std::ios::binary);
    char buffer[4];

    if (!stream.read(buffer, 4))
        throw ERRMSG("Failed to read");
    //uintG numAssets = to_native_endian32(buffer, Endian::BIG);


}

Scene* Scene::parse(const std::string &path, std::string name)
{
    Scene *scene = new Scene();
    Scene::activeScene = scene;
    parse_scene(scene, path);
    scene->initialize(name);
    return 0;
}