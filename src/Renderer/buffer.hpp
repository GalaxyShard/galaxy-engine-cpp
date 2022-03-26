#pragma once
#include <Galaxy/OS/defines.hpp>
#include <vector>
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
    unsigned int rendererID, indexCount;
public:
    IndexBuffer(const void *data, unsigned int bytes, bool isStatic=0);
    ~IndexBuffer();
    void update_data(const void* data, unsigned int bytes);

    void bind() const;
    void unbind() const;
    inline unsigned int get_count() { return indexCount; }
};
class VertexBuffer : public GraphicsBuffer
{
private:
    unsigned int rendererID;
public:
    VertexBuffer(const void *data, unsigned int size, bool isStatic=0);
    ~VertexBuffer();

    void update_data(const void *data, unsigned int count);
    void bind() const;
    void unbind() const;
};
// hack: uses raw OpenGL values
enum class AttributeType
{
    NONE = 0,
    BYTE = 0x1400, 
    UBYTE = 0x1401,
    SHORT = 0x1402,
    USHORT = 0x1403,
    INT = 0x1404,
    UINT = 0x1405,
    FLOAT = 0x1406,
    DOUBLE = 0x140A,
    HALF = 0x140B
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
    unsigned int rendererID;
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