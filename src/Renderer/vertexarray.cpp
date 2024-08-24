#include <Renderer/buffer.hpp>
#include <gldebug.hpp>
#include <unordered_map>

std::unordered_map<AttributeType, int> typeToBytes =
{
    {AttributeType::BYTE,1},
    {AttributeType::UBYTE,1},
    {AttributeType::SHORT,2},
    {AttributeType::USHORT,2},
    {AttributeType::INT,4},
    {AttributeType::UINT,4},
    {AttributeType::FLOAT,4}
};
void VertexLayout::add_attribute(VertexAttribute a)
{
    attributes.push_back(a);
    stride += a.count * typeToBytes[a.type];
}

VertexArray::VertexArray()
{
#if OS_IOS
    GLCall(glGenVertexArraysOES(1, &rendererID));
#elif USE_GLFW
    GLCall(glGenVertexArrays(1, &rendererID));
#endif
}
VertexArray::~VertexArray()
{
#if OS_IOS
    GLCall(glDeleteVertexArraysOES(1, &rendererID));
#elif USE_GLFW
    GLCall(glDeleteVertexArrays(1, &rendererID));
#endif
}
void VertexArray::add_buffer(IndexBuffer &buffer)
{
#if OS_WEB
    ibo = &buffer;
#else
    (void)buffer;
#endif
}
void VertexArray::add_buffer(VertexBuffer &buffer)
{
#if OS_WEB
    vbo = &buffer;
#else
    (void)buffer;
#endif
}
static void apply_layout(const VertexLayout &layout)
{
    long offset = 0;
    int index = 0;
    for (auto &a : layout.get_attributes())
    {
        GLCall(glVertexAttribPointer(index, a.count, (int)a.type, false, layout.get_stride(), (void*)offset));
        GLCall(glEnableVertexAttribArray(index));
    
        offset += a.count * typeToBytes[a.type];
        ++index;
    }
}
void VertexArray::add_layout(VertexLayout layout)
{
#if OS_WEB
    vertexLayout = layout;
#else
    apply_layout(layout);
#endif
}
void VertexArray::bind() const
{
#if OS_IOS
    GLCall(glBindVertexArrayOES(rendererID));
#elif USE_GLFW
    GLCall(glBindVertexArray(rendererID));
#elif OS_WEB
    if (ibo && vbo)
    {
        ibo->bind();
        vbo->bind();
        apply_layout(vertexLayout);
    }
#endif
}
void VertexArray::unbind() const
{
#if OS_IOS
    GLCall(glBindVertexArrayOES(0));
#elif USE_GLFW
    GLCall(glBindVertexArray(0));
#elif OS_WEB
    if (ibo) ibo->unbind();
    if (vbo) vbo->unbind();
#endif
}