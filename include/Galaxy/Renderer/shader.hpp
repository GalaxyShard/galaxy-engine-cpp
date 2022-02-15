#pragma once
#include <unordered_map>
#include <Galaxy/OS/defines.hpp>
struct Vector3;
struct Vector4;
struct Matrix4x4;

#if OS_MOBILE
    #define SHADER_FOLDER "/shaders-es"
#else
    #define SHADER_FOLDER "/shaders-gl"
#endif
template<typename T>
class AssetRef;

struct Shader
{
    private:
        unsigned int vertex = 0, fragment = 0, program = 0;

        std::unordered_map<const char*, int> uniformCache = {};
        void parse(std::istream &stream);
        unsigned int create_program();

        friend class Material;
    public:
        int u_location(const char *uniform);
        void bind();
        void set_uniform3f(const char *uniform, Vector3 v);
        void set_uniform4f(const char *uniform, Vector4 v);
        void set_uniform_mat4x4(const char *uniform, Matrix4x4 v);
        void set_uniform1i(const char *uniform, int v);

        Shader(const std::string &path);
        ~Shader();
        static AssetRef<Shader> load(const std::string &path);

        Shader(const Shader&) = delete;
        void operator=(const Shader&) = delete;
};