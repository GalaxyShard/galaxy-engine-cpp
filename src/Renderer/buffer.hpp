#pragma once
#include <Galaxy/OS/defines.hpp>
#include <vector>
#include <gldebug.hpp>

class GraphicsBuffer
{
public:
    GraphicsBuffer() { }
    GraphicsBuffer(const GraphicsBuffer&) = delete;
    void operator=(const GraphicsBuffer&) = delete;
};
class IndexBuffer : public GraphicsBuffer
{
private:
    uint32_t rendererID, indexCount;
public:
    IndexBuffer();
    ~IndexBuffer();
    void update_data(const void* data, uint32_t bytes);

    void bind() const;
    void unbind() const;
    inline uint32_t get_count() { return indexCount; }
};
class VertexBuffer : public GraphicsBuffer
{
private:
    uint32_t rendererID;
public:
    VertexBuffer();
    ~VertexBuffer();

    void update_data(const void *data, uint32_t count);
    void bind() const;
    void unbind() const;
};
enum class AttributeType
{
    NONE = 0,
    BYTE = GL_BYTE, 
    UBYTE = GL_UNSIGNED_BYTE,
    SHORT = GL_SHORT,
    USHORT = GL_UNSIGNED_SHORT,
    INT = GL_INT,
    UINT = GL_UNSIGNED_INT,
    FLOAT = GL_FLOAT
};
struct VertexAttribute
{
    AttributeType type;
    int count = 0;
    VertexAttribute(AttributeType t, int c) : type(t), count(c) { }
};
class VertexLayout
{
private:
    std::vector<VertexAttribute> attributes;
    int stride = 0;
public:

    void add_attribute(VertexAttribute a);
    inline const std::vector<VertexAttribute> get_attributes() const { return attributes; }
    inline int get_stride() const { return stride; }
};
class VertexArray : public GraphicsBuffer
{
private:
#if OS_WEB
    IndexBuffer *ibo = 0;
    VertexBuffer *vbo = 0;
    VertexLayout vertexLayout;
#else
    uint32_t rendererID;
#endif
public:
    VertexArray();
    ~VertexArray();
    void add_buffer(IndexBuffer &buffer);
    void add_buffer(VertexBuffer &buffer);
    void add_layout(VertexLayout layout);
    
    void bind() const;
    void unbind() const;
};