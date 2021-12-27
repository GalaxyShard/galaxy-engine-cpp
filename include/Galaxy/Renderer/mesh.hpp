#pragma once
#include "Galaxy/Math/vector.hpp"
#include <vector>

class VertexArray;
class VertexBuffer;
class IndexBuffer;
template<typename T>
class AssetRef;

struct Vertex
{
    Vector3 pos;
    Vector2 texCoord;
    Vertex() { }
    Vertex(float x, float y, float z) : pos(x, y, z) { }
    Vertex(Vector3 pos, Vector2 texCoord = Vector2()) : pos(pos), texCoord(texCoord) { }
};
class Mesh
{
    private:
        std::unique_ptr<VertexArray> varray;
        std::unique_ptr<VertexBuffer> vbuffer;
        std::unique_ptr<IndexBuffer> ibuffer;
        friend class Renderer;

    public:
        std::vector<Vertex> verts;
        std::vector<unsigned int> tris;

        void initialize_mesh();
        void refresh_mesh();

        Mesh();
        Mesh(std::vector<Vertex> verts, std::vector<unsigned int> tris);
        ~Mesh();

        Mesh(const Mesh&) = delete;
        void operator=(const Mesh&) = delete;

        //static Mesh* from_obj(const std::string &path);
        static AssetRef<Mesh> from_obj(const std::string &path);
};