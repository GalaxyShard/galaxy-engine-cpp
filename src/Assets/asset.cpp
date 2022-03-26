#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/OS/defines.hpp>
#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Renderer/texture.hpp>
#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Audio/audio.hpp>
#include <unordered_map>
#include <assert.h>

#if OS_MAC
    #include <filesystem>
#endif


const char *get_resource_path_platform();

#if OS_WEB
const char *get_resource_path_platform()
{
    return "bin/web/res";
}
#endif
extern const char *gameName;

std::string Assets::resource_path()
{ return std::string(get_resource_path_platform()); }

std::string Assets::path()
{ return resource_path()+"/assets"; }

std::string Assets::gpath()
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
    #elif OS_WEB
    auto path = std::string(); // TODO
    assert(false && "Error: no data path implemented");
    #else
    static_assert(false, "Platform not implemented");
    #endif
    return path;
}
template<typename T>
AssetRef<T>::AssetRef() {  }
template<typename T>
AssetRef<T>::AssetRef(T *asset, const std::string &ipath) : path(ipath)
{
    data = asset;
    refCount = new unsigned short(1);

    WeakAssetRef<T> weakRef;
    weakRef.data = data;
    weakRef.refCount = refCount;
    loadedAssets->insert(std::make_pair(path, weakRef));
}
template<typename T>
AssetRef<T>::~AssetRef()
{
    if (!valid()) return;
    --(*refCount);
    if ((*refCount) == 0)
    {
        if (loadedAssets)
            loadedAssets->erase(path);
        delete data;
        delete refCount;
    }
}
template<typename T>
AssetRef<T>& AssetRef<T>::operator=(const AssetRef<T> &other)
{
    if (this != &other){
        this->~AssetRef();
        refCount = other.refCount;
        data = other.data;
        path = other.path;

        if (valid())
            ++(*refCount);
    }
    return *this;
}
template<typename T>
AssetRef<T>::AssetRef(const AssetRef<T> &other)
{
    refCount = other.refCount;
    data = other.data;
    path = other.path;

    if (valid())
        ++(*refCount);
}
template<typename T>
AssetRef<T>::AssetRef(AssetRef<T> &&other)
{
    data = other.data;
    refCount = other.refCount;
    path = other.path;

    // null other so the destructor doesnt cause a memory error
    other.data = 0;
    other.refCount = 0;
    other.path = "";
}
template<typename T>
AssetRef<T>& AssetRef<T>::operator=(AssetRef<T> &&other)
{
    if (this != &other)
    {
        this->~AssetRef();
        data = other.data;
        refCount = other.refCount;
        path = other.path;

        // null other so the destructor doesnt cause a memory error
        other.data = 0;
        other.refCount = 0;
        other.path = "";
    }
    return *this;
}
template<typename T>
bool AssetRef<T>::is_loaded(const std::string &path)
{
    return loadedAssets->count(path);
}
template<typename T>
AssetRef<T> AssetRef<T>::get_loaded(const std::string &path)
{
    WeakAssetRef<T> &weakAsset = (*loadedAssets)[path];
    AssetRef<T> asset = AssetRef<T>();
    asset.refCount = weakAsset.refCount;
    asset.data = weakAsset.data;
    asset.path = path;
    if (asset.valid())
        ++(*asset.refCount);
    return asset;
}
template class AssetRef<Mesh>;
template class AssetRef<Shader>;
template class AssetRef<Texture>;
template class AssetRef<AudioData>;