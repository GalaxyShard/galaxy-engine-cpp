#pragma once
#include "Galaxy/Math/vector.hpp"
#include <vector>
#include <Galaxy/OS/defines.hpp>

class VertexArray;
class VertexBuffer;
class IndexBuffer;
template<typename T>
class AssetRef;

struct Vertex
{
    Vector3 pos;
    Vector2 texCoord;
    Vector3 normal;
    Vertex() { }
    Vertex(float x, float y, float z) : pos(x, y, z) { }
    Vertex(Vector3 pos, Vector2 texCoord = Vector2(), Vector3 normal = Vector3()) : pos(pos), texCoord(texCoord), normal(normal) { }
};
class Mesh
{
private:
    VertexArray *varray;
    VertexBuffer *vbuffer;
    IndexBuffer *ibuffer;
    friend class Renderer;
public:
    std::vector<Vertex> verts;
    std::vector<unsigned int> tris;
    Vector3 aabbMin, aabbMax;

    void initialize_mesh();
    void refresh_mesh();
    void calculate_bounds();
    void calculate_normals();

    Mesh();
    Mesh(std::vector<Vertex> verts, std::vector<unsigned int> tris);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    void operator=(const Mesh&) = delete;

    static AssetRef<Mesh> load_obj(const std::string &path);
};