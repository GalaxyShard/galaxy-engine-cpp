#include <Renderer/buffer.hpp>
#include <gldebug.hpp>
VertexBuffer::VertexBuffer(const void *data, unsigned int size, bool isStatic)
{
    GLCall(glGenBuffers(1, &rendererID));
    bind();
    unsigned int drawMode = (isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, drawMode));
}
VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &rendererID));
}
void VertexBuffer::update_data(const void *data, unsigned int size)
{
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}
void VertexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, rendererID));
}
void VertexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}