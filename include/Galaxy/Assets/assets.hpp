#pragma once
#include <unordered_map>
#include <fstream>
namespace Assets
{
    // Call after writing to files
    void sync_files();

    std::string data_path();
    std::string resource_path();
    std::string path();
    std::string gpath();
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
    static std::unordered_map<std::string, WeakAssetRef<T>> *loadedAssets;
    T *data = 0;
    unsigned short *refCount = 0;
    std::string path;
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
class Mesh;
class Shader;
class Texture;
class AudioData;
extern template class AssetRef<Mesh>;
extern template class AssetRef<Shader>;
extern template class AssetRef<Texture>;
extern template class AssetRef<AudioData>;

template<typename T>
std::unordered_map<std::string, WeakAssetRef<T>> *AssetRef<T>::loadedAssets = new std::unordered_map<std::string, WeakAssetRef<T>>();
