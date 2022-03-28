#include <Renderer/buffer.hpp>
#include <gldebug.hpp>
IndexBuffer::IndexBuffer()
{
    indexCount = 0;
    GLCall(glGenBuffers(1, &rendererID));
}
IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &rendererID));
}
void IndexBuffer::update_data(const void* data, unsigned int bytes)
{
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bytes, data, GL_STATIC_DRAW));
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