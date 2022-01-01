#include <Galaxy/Renderer/mesh.hpp>
#include <Galaxy/Assets/assets.hpp>
#include <Renderer/buffer.hpp>
#include <vector>
#include <fstream>
#include <sstream>

#include <iostream>

using std::vector;
Mesh::Mesh() { } // for smart ptr
Mesh::Mesh(vector<Vertex> verts, vector<unsigned int> tris) : verts(verts), tris(tris)
{
    initialize_mesh();
}
void Mesh::initialize_mesh()
{
    bool isStatic = false; // unimplemented

    varray = std::make_unique<VertexArray>();
    vbuffer = std::make_unique<VertexBuffer>(verts.data(), sizeof(Vertex)*verts.size(), isStatic);
    ibuffer = std::make_unique<IndexBuffer>(tris.data(), tris.size(), isStatic);

    varray->add_buffer(*vbuffer);
    varray->add_buffer(*ibuffer);

    auto layout = VertexLayout();
    layout.add_attribute({AttributeType::FLOAT, 3}); // pos
    layout.add_attribute({AttributeType::FLOAT, 2}); // texCoords
    varray->add_layout(layout);

    
}
Mesh::~Mesh() = default; // for smart ptr
void Mesh::refresh_mesh()
{
    varray->bind();
    vbuffer->update_data(verts.data(), sizeof(Vertex)*verts.size());
    ibuffer->update_data(tris.data(), tris.size());
}
static vector<std::string> split_str(const std::string &str, const char c)
{
    vector<std::string> split;
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

    vector<Vector2> texCoords;
    while(std::getline(stream, line))
    {
        const char &c0 = line[0];
        switch (c0)
        {
        case '#': continue;
        case 'o': assert(false); // multiple meshes not supported
        //case 'o': continue; // combine all objects into one model for now
        
        case 'v':
        {
            const char &c1 = line[1];
            vector<std::string> v = split_str(line, ' ');
            if (c1 == ' ') // vertex
            {
                mesh->verts.push_back(Vertex(
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
            {}
            break;
        }
        case 'f': // face
        {
            vector<std::string> tris = split_str(line, ' ');
            tris.erase(tris.begin());
            if (tris.size() > 3)
            {
                // TODO: triangulate
                assert(false);
            }
            for (auto str : tris)
            {
                vector<std::string> vert_tex_norm = split_str(str, '/');
                // no texcoords or normals, possibly calculate normals automatically
                if (vert_tex_norm.size() == 1)
                {
                    mesh->tris.push_back(std::stoi(str)-1);
                }
                else
                {
                    auto vert = vert_tex_norm[0];
                    int vertIndex = std::stoi(vert)-1;
                    mesh->tris.push_back(vertIndex);
                    
                    auto tex = vert_tex_norm[1];
                    if (!tex.empty())
                    {
                        int uvIndex = std::stoi(tex)-1;
                        if (mesh->verts[vertIndex].texCoord != texCoords[uvIndex])
                        {
                            assert(mesh->verts[vertIndex].texCoord == Vector2(0,0));
                            mesh->verts[vertIndex].texCoord = texCoords[uvIndex];
                        }
                    }
                    //auto norm = vert_tex_norm[2];
                    //if (!norm.empty()) mesh->normals.push_back(std::stoi(norm)-1);
                }
            }

            break;
        }
        case 's': // smoothing
            smoothOn = (line[2] == '1');
            break;
        default:
            break;
        }
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