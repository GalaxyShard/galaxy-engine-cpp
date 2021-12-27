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
class AssetRef
{
private:
    static std::unordered_map<const std::string*, AssetRef<T>*> loadedAssets;
    T *data = 0;
    unsigned short *refCount = 0;
    std::string path;

    friend class Mesh;
    friend class Texture;
    friend class Shader;
public:
    T* get() { return data; }
    bool valid() { return (*refCount) != 0; }
    AssetRef();
    AssetRef(T *asset, const std::string &ipath);
    ~AssetRef();
    
    void operator=(const AssetRef &);
    AssetRef(const AssetRef &);
};
template<typename T>
std::unordered_map<const std::string*, AssetRef<T>*> AssetRef<T>::loadedAssets = {};
template<typename T>
AssetRef<T>::AssetRef()
{
    refCount = new unsigned short(0);
}
template<typename T>
AssetRef<T>::AssetRef(T *asset, const std::string &ipath) : path(ipath)
{
    data = asset;
    refCount = new unsigned short(1);
    loadedAssets[&path] = this;
}
template<typename T>
AssetRef<T>::~AssetRef()
{
    if (!valid()) return;
    --(*refCount);
    if ((*refCount) == 0)
    {
        if (data) // check if there is an asset or if it is a null asset
            loadedAssets.erase(&path);
        delete data;
        delete refCount;
    }
}
template<typename T>
void AssetRef<T>::operator=(const AssetRef<T> &other)
{
    this->~AssetRef();
    refCount = other.refCount;
    data = other.data;

    if ((*refCount) != 0)
        ++(*refCount);
}
template<typename T>
AssetRef<T>::AssetRef(const AssetRef<T> &other)
{
    refCount = other.refCount;
    data = other.data;

    if ((*refCount) != 0)
        ++(*refCount);
}