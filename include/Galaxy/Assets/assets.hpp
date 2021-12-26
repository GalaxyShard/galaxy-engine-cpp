#pragma once
#include <fstream>
struct FileContent
{
    unsigned int length;
    const char *str;

    ~FileContent();
};
// use for reference counting on assets
// putting in a map wont work though
//template<typename T>
//class AssetRef
//{
//private:
//public:
//    T *data;
//    //T *get() { return (T*)data; }

//    AssetRef();
//    ~AssetRef();
//};

namespace Assets
{
    std::string resource_path();
    std::string asset_path();
    std::string gasset_path();

    std::string data_path();
    FileContent file_contents(const char *path, bool assertOnFail = true);
    FileContent file_contents(const std::string &path, bool assertOnFail = true);

    // returns the asset requested, or null if not found
    //template<typename T>
    //T get_asset(std::string path);
    
    //void release_asset(std::string path);
    //void unload_asset(std::string path);

    /*
    in constructors that load assets (eg texture, shader, mesh):
    Assets::add_asset("identifier", this);
    Assets::add_asset(path, this);

    
    in code
    Assets::get_asset<Texture>("identifier");
    or
    Texture::create("path");
    
    */
//   void add_asset(std::string name, Asset asset);
}

//class Asset
//{
//private:
//    const char *path;

//public:
//    Asset(const char *path);

//    std::ifstream get_stream() const;
//    FileContent get_contents() const;
//};

//namespace Galaxy
//{
    //#define GALAXY_ASSET_PATH "/Users/galaxyshard/Projects/Galaxy Engine/assets"
    //extern const char *assetPath;
//}