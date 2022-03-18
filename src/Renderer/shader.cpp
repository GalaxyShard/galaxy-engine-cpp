#include <Galaxy/Renderer/shader.hpp>
#include <Galaxy/Assets/assets.hpp>
#include <Galaxy/Math/vector.hpp>
#include <Galaxy/Math/matrix.hpp>
#include <iostream>

#include <sstream>
#include <fstream>
#include <internalinit.hpp>
#include <gldebug.hpp>
static unsigned int compile_shader(unsigned int type, const char *source)
{
    unsigned int shader = GLCall(glCreateShader(type));
    GLCall(glShaderSource(shader, 1, &source, nullptr));
    GLCall(glCompileShader(shader));

    int success;
    GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success)
    {
        int logSize;
        GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize));
        auto log = std::make_unique<char[]>(logSize);
        GLCall(glGetShaderInfoLog(shader, logSize, nullptr, log.get()));
        logmsg("ERROR compiling shader\n%o\nShader: \'%o\'\n", log.get(), source);

        GLCall(glDeleteShader(shader));
        assert(false);
    }
    return shader;
}
static std::unique_ptr<Shader> fallback;

void Shader::parse(std::istream &stream)
{
    std::string source[2];
    std::string temp;

    const int VERTEX = 0, FRAGMENT = 1;
    int currentShader = -1;
    
    constexpr auto npos = std::string::npos;

    while (std::getline(stream, temp))
    {
        if (temp.find("#shader") != npos)
        {
            if (temp.find("vertex") != npos) currentShader = VERTEX;
            else if (temp.find("fragment") != npos) currentShader = FRAGMENT;
            else std::cout << "ERROR parsing shader: type not found" << std::endl;
            continue;
        }
        else if (currentShader == -1) continue;
        source[currentShader].append(temp).append("\n");
    }
    vertex = compile_shader(GL_VERTEX_SHADER, source[VERTEX].c_str());
    fragment = compile_shader(GL_FRAGMENT_SHADER, source[FRAGMENT].c_str());

    program = create_program();
}
Shader::Shader(const std::string& path)
{
    auto stream = std::ifstream(path, std::ios::binary);
    parse(stream);
}
AssetRef<Shader> Shader::load(const std::string &path)
{
    if (AssetRef<Shader>::is_loaded(path))
        return AssetRef<Shader>::get_loaded(path);
    return AssetRef(new Shader(path), path);
}
void Shader::bind() { GLCall(glUseProgram(program)); }
unsigned int Shader::create_program()
{
    GLCall(unsigned int program = glCreateProgram());
    if (vertex == 0 || fragment == 0)
    {
        if (fallback == nullptr)
        {
            logerr("Error creating program and fallback is null\n");
            assert(false);
        }
        vertex = fallback->vertex;
        fragment = fallback->fragment;
    }
    GLCall(glAttachShader(program, vertex));
    GLCall(glAttachShader(program, fragment));

    #if USE_GLFM
        glBindAttribLocation(program, 0, "pos");
        glBindAttribLocation(program, 1, "texCoord");
        glBindAttribLocation(program, 2, "normal");
    #endif
    GLCall(glLinkProgram(program));
    
    int success;
    GLCall(glGetProgramiv(program, GL_LINK_STATUS, &success));
    if (!success)
    {
        int logSize;
        GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize));
        auto log = std::make_unique<char[]>(logSize);
        GLCall(glGetProgramInfoLog(program, logSize, nullptr, log.get()));
        logerr("ERROR creating shader program\n%o\n", log.get());
        GLCall(glDeleteProgram(program));
        assert(false);
    }
    GLCall(glValidateProgram(program));
    return program;
}
int Shader::u_location(const char *uniform)
{
    if (uniformCache.count(uniform) == 0)
    {
        GLCall(int location = glGetUniformLocation(program, uniform));
        uniformCache[uniform] = location;
    }
    return uniformCache[uniform];
}
#define DEFINE_SET_UNIFORM(func, ...) int location = u_location(uniform); \
    if (location != -1) GLCall(glUniform##func(location, __VA_ARGS__));
void Shader::set_uniform3f(const char *uniform, Vector3 v)
{
    DEFINE_SET_UNIFORM(3f, v.x, v.y, v.z);
}
void Shader::set_uniform4f(const char *uniform, Vector4 v)
{
    DEFINE_SET_UNIFORM(4f, v.x, v.y, v.z, v.w);
}
void Shader::set_uniform_mat4x4(const char *uniform, Matrix4x4 v)
{
    DEFINE_SET_UNIFORM(Matrix4fv, 1, false, v.transpose().value_ptr());
}
void Shader::set_uniform1i(const char *uniform, int v)
{
    DEFINE_SET_UNIFORM(1i, v);
}
#undef DEFINE_SET_UNIFORM
Shader::~Shader()
{
    GLCall(glDeleteShader(vertex));
    GLCall(glDeleteShader(fragment));
    GLCall(glDeleteProgram(program));
}

static void init()
{
    fallback = std::make_unique<Shader>(Assets::gpath()+SHADER_FOLDER+"/fallback.shader");
}
INTERNAL_INIT_FUNC(init);