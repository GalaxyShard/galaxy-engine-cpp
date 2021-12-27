#include <unordered_map>
#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/OS/defines.hpp>
#ifdef OS_MAC
    #include <filesystem>
#endif
const char *get_resource_path_platform();
const char *get_bundle_identifier();
extern const char *gameName;

//std::unordered_map<std::string, AssetRef<>>;

std::string Assets::resource_path()
{ return std::string(get_resource_path_platform()); }

std::string Assets::asset_path()
{ return resource_path()+"/assets"; }

std::string Assets::gasset_path()
{ return resource_path()+"/galaxy_assets"; }

std::string Assets::data_path()
{
    #if OS_MAC
    // HOME on OSX is /Users/username
    auto path = std::string() +
        getenv("HOME") + "/Library/Application Support/gs_" + gameName;
    std::filesystem::create_directory(path);
    #elif OS_IOS
    // HOME on iOS is the application folder
    auto path = std::string() + getenv("HOME") + "/Documents";
    #else
    static_assert(false, "Platform not implemented");
    #endif
    return path;
}
FileContent::~FileContent()
{
    delete[] str;
}
FileContent Assets::file_contents(const char *path, bool assertOnFail)
{
    auto stream = std::ifstream(path, std::ios::binary | std::ios::ate);
    //assert(stream.good());
    if (!stream.good())
    {
        if (assertOnFail) assert(false);
        else return {0, nullptr};
    }
    unsigned int length = stream.tellg();

    stream.seekg(0);
    char *contents = new char[length+1];
    stream.read(contents, length);

    assert(stream.good());
    strcat(contents, ""); // add null terminator
    return {length+1, contents};
}
FileContent Assets::file_contents(const std::string &path, bool assertOnFail)
{
    return file_contents(path.c_str(), assertOnFail);
}