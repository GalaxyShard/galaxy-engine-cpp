#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Assets/assets.hpp>
#include <iostream>

#include <Renderer/buffer.hpp>
#include <vector>
#include <fstream>
#include <sstream>


//using std::vector;
Mesh::Mesh() { } // for smart ptr
Mesh::Mesh(std::vector<Vertex> verts, std::vector<unsigned short> tris) : verts(verts), tris(tris)
{
    initialize_mesh();
}
void Mesh::initialize_mesh()
{
    //printf("triangles: %lu\n", tris.size());
    Debug::log("triangles: %o\n", tris.size());
    bool isStatic = true;

    varray = std::make_unique<VertexArray>();
    vbuffer = std::make_unique<VertexBuffer>(verts.data(), sizeof(Vertex)*verts.size(), isStatic);
    ibuffer = std::make_unique<IndexBuffer>(tris.data(), tris.size()*sizeof(unsigned short), isStatic);

    varray->add_buffer(*vbuffer);
    varray->add_buffer(*ibuffer);

    auto layout = VertexLayout();
    layout.add_attribute({AttributeType::FLOAT, 3}); // pos
    layout.add_attribute({AttributeType::FLOAT, 2}); // texCoords
    layout.add_attribute({AttributeType::FLOAT, 3}); // normals
    varray->add_layout(layout);

    calculate_bounds();
}
Mesh::~Mesh() = default; // for smart ptr
void Mesh::refresh_mesh()
{
    vbuffer->bind();
    vbuffer->update_data(verts.data(), sizeof(Vertex)*verts.size());
    ibuffer->bind();
    ibuffer->update_data(tris.data(), tris.size()*sizeof(unsigned short));
}
void Mesh::calculate_bounds()
{
    if (!verts.size())
        return;
    aabbMin = Vector3(Math::INF, Math::INF, Math::INF);
    aabbMax = Vector3(-Math::INF, -Math::INF, -Math::INF);
    //aabbMax = Vector3(Math::NEGINF, Math::NEGINF, Math::NEGINF);
    using std::min; using std::max;
    for (int i = 0; i < verts.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            aabbMin[j] = min(aabbMin[j], verts[i].pos[j]);
            aabbMax[j] = max(aabbMax[j], verts[i].pos[j]);
        }
    }
}
void Mesh::calculate_normals()
{
}
static std::vector<std::string> split_str(const std::string &str, const char c)
{
    std::vector<std::string> split;
    std::string temp;
    std::stringstream ss = std::stringstream(str);

    while (std::getline(ss, temp, c)) split.push_back(std::move(temp));
    return split;
}
void parse_obj(Mesh *&mesh, std::istream &stream)
{
/*
    https://andrewnoske.com/wiki/OBJ_file_format#Example_2a:_Square_based_pyramid
    f (face) indices seperated by a space, eg 'f vert1 vert2 vert3'
    can include normals and texcoords with vertex/texcoord/normal OR vertex//normal
    eg 'f vert1/tc1/norm1 vert2/tc2/norm2 vert3/tc3/norm3'

    s (smoothing) '1' or 'off', used to smooth normals
    vn (normals) three floats seperated by a space
    vt (texture) two coords seperated by a space
    o (object) represents the start of another object in the same file
    v (vertex), three coords seperated by a space
    # (comment)
*/
    std::string line;
    int smoothOn = -1;

    std::vector<Vector2> texCoords;
    std::vector<Vector3> normals, vertices;
    //std::vector<int> triangles;
    struct Face
    {
        int triangle[3] = {-1,-1,-1};
        int uv[3] = {-1,-1,-1};
        int normal[3] = {-1,-1,-1};
    };
    std::vector<Face> faces;

    while(std::getline(stream, line))
    {
        const char &c0 = line[0];
        switch (c0)
        {
        case '#': continue;
        case 'o': throw("Multiple meshes not supported");

        case 'v':
        {
            const char &c1 = line[1];
            std::vector<std::string> v = split_str(line, ' ');
            if (c1 == ' ') // vertex
            {
                //mesh->verts.push_back(Vertex(
                vertices.push_back(Vector3(
                    std::stof(v[1]),
                    std::stof(v[2]),
                    std::stof(v[3])));
            }
            else if (c1 == 't') // texture/uv
            {
                texCoords.push_back(Vector2(
                    std::stof(v[1]),
                    std::stof(v[2])));
            }
            else if (c1 == 'n') // normals
            {
                normals.push_back(Vector3(
                    std::stof(v[1]),
                    std::stof(v[2]),
                    std::stof(v[3])
                ));
            }
            break;
        }
        case 'f': // face
        {
            std::vector<std::string> tris = split_str(line, ' ');
            tris.erase(tris.begin());
            if (tris.size() != 3)
            {
                // TODO: triangulate
                //assert(false);
                throw("Cannot triangulate mesh automatically");
            }
            Face face;
            int i = 0;
            for (auto str : tris)
            {
                std::vector<std::string> vert_tex_norm = split_str(str, '/');
                // no uvs or normals, possibly calculate normals automatically
                if (vert_tex_norm.size() == 1)
                {
                    face.triangle[i] = std::stoi(str)-1;
                    //mesh->tris.push_back(std::stoi(str)-1);
                    //triangles.push_back(std::stoi(str)-1);
                }
                else
                {
                    auto vert = vert_tex_norm[0];
                    int vertIndex = std::stoi(vert)-1;
                    face.triangle[i] = vertIndex;
                    //mesh->tris.push_back(vertIndex);
                    //triangles.push_back(vertIndex);
                    
                    auto tex = vert_tex_norm[1];
                    if (!tex.empty())
                    {
                        int uvIndex = std::stoi(tex)-1;
                        face.uv[i] = uvIndex;
                        //if (mesh->verts[vertIndex].texCoord != texCoords[uvIndex])
                        //{
                            //assert(mesh->verts[vertIndex].texCoord == Vector2(0,0));
                            //mesh->verts[vertIndex].texCoord = texCoords[uvIndex];
                        //}
                        //mesh->verts[vertIndex].texCoord = texCoords[uvIndex];
                    }
                    if (vert_tex_norm.size() < 3)
                    {
                        ++i;
                        continue;
                    }
                    auto normal = vert_tex_norm[2];
                    if (!normal.empty())
                    {
                        int normalIndex = std::stoi(normal)-1;
                        face.normal[i] = normalIndex;
                        //if (mesh->verts[vertIndex].normal != normal[normalIndex])
                        //{
                            //assert(mesh->verts[vertIndex].normal == Vector3(0,0,0));
                            //mesh->verts[vertIndex].normal = normals[normalIndex];
                        //}
                        //mesh->verts[vertIndex].normal = normals[normalIndex];
                    }
                }
                ++i;
            }
            faces.push_back(face);
            break;
        }
        case 's': // smoothing
            smoothOn = (line[2] == '1');
            break;
        default:
            break;
        }
    }
    int i = 0;
    for (auto &face : faces)
    {
        for (int j = 0; j < 3; ++j)
        {
            Vertex vert = Vertex(vertices[face.triangle[j]]);
            if (face.uv[j] != -1) vert.texCoord = texCoords[face.uv[j]];
            if (face.normal[j] != -1) vert.normal = normals[face.normal[j]];
            int vertIndex = mesh->verts.size();
            mesh->verts.push_back(vert);
            mesh->tris.push_back(vertIndex);

            // todo: fix ordering
            //mesh->tris.push_back(face.triangle[j]);
            //mesh->tris.push_back(i+j);
        }
        ++i;
    }
}
AssetRef<Mesh> Mesh::from_obj(const std::string &path)
{
    if (AssetRef<Mesh>::is_loaded(path))
        return AssetRef<Mesh>::get_loaded(path);

    Mesh *mesh = new Mesh();
    auto stream = std::ifstream(path, std::ios::binary);
    parse_obj(mesh, stream);
    mesh->initialize_mesh();

    return AssetRef(mesh, path);
}