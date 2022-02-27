#include <Renderer/buffer.hpp>
#include <gldebug.hpp>
IndexBuffer::IndexBuffer(const void *data, unsigned int bytes, bool isStatic)
{
    indexCount = bytes;
    GLCall(glGenBuffers(1, &rendererID));
    bind();
    unsigned int drawMode = (isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, drawMode));
    //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes*sizeof(unsigned int), data, drawMode));
}
IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &rendererID));
}
void IndexBuffer::update_data(const void* data, unsigned int bytes)
{
    bind();
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW));
    //GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes*sizeof(unsigned int), data, GL_STATIC_DRAW));
    indexCount = bytes;
}
void IndexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
}
void IndexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}