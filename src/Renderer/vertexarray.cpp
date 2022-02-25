#include <Renderer/buffer.hpp>
#include <gldebug.hpp>
#include <unordered_map>

extern std::unordered_map<AttributeType, int> typeToBytes;
VertexArray::VertexArray()
{
#if OS_MOBILE
    GLCall(glGenVertexArraysOES(1, &rendererID));
//#elif OS_WEB

#else
    GLCall(glGenVertexArrays(1, &rendererID));
#endif
    bind();
}
VertexArray::~VertexArray()
{
#if OS_MOBILE
    GLCall(glDeleteVertexArraysOES(1, &rendererID));
//#elif OS_WEB
#else

    GLCall(glDeleteVertexArrays(1, &rendererID));
#endif
}
void VertexArray::add_buffer(IndexBuffer &buffer)
{
    bind();
    buffer.bind();
}
void VertexArray::add_buffer(VertexBuffer &buffer)
{
    bind();
    buffer.bind();
}
void VertexArray::add_layout(VertexLayout layout)
{
    bind();
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
void VertexArray::bind() const
{
#if OS_MOBILE
    GLCall(glBindVertexArrayOES(rendererID));
//#elif OS_WEB

#else
    GLCall(glBindVertexArray(rendererID));
#endif
}
void VertexArray::unbind() const
{
#if OS_MOBILE
    GLCall(glBindVertexArrayOES(0));
//#elif OS_WEB
#else
    GLCall(glBindVertexArray(0));
#endif
}