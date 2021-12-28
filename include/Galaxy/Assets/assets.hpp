#pragma once
#include <unordered_map>
#include <fstream>
struct FileContent
{
    unsigned int length;
    const char *str;

    ~FileContent();
};
namespace Assets
{
    std::string resource_path();
    std::string asset_path();
    std::string gasset_path();

    std::string data_path();
    FileContent file_contents(const char *path, bool assertOnFail = true);
    FileContent file_contents(const std::string &path, bool assertOnFail = true);

}
template<typename T>
struct WeakAssetRef
{
    T *data = 0;
    unsigned short *refCount = 0;
};
template<typename T>
class AssetRef
{
private:
    static std::unique_ptr<std::unordered_map<std::string, WeakAssetRef<T>>> loadedAssets;
    T *data = 0;
    unsigned short *refCount = 0;
    std::string path;

    //friend class Mesh;
    //friend class Texture;
    //friend class Shader;
public:
    T* get() { return data; }
    bool valid() { return data && (*refCount) != 0; }

    static bool is_loaded(const std::string &path);
    static AssetRef<T> get_loaded(const std::string &path);

    AssetRef();
    AssetRef(T *asset, const std::string &ipath);
    ~AssetRef();
    
    AssetRef<T>& operator=(const AssetRef &);
    AssetRef(const AssetRef &);
    AssetRef<T>& operator=(AssetRef &&);
    AssetRef(AssetRef &&);

    T* operator->() { return data; }
    T& operator*() { return *data; }
};
template<typename T>
std::unique_ptr<std::unordered_map<std::string, WeakAssetRef<T>>> AssetRef<T>::loadedAssets = std::make_unique<std::unordered_map<std::string, WeakAssetRef<T>>>();
