#include <Renderer/buffer.hpp>
#include <debug.hpp>
IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count, bool isStatic)
{
    indexCount = count;
    GLCall(glGenBuffers(1, &rendererID));
    bind();
    unsigned int drawMode = (isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, drawMode));
}
IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &rendererID));
}
void IndexBuffer::update_data(const void* data, unsigned int count)
{
    bind();
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_DYNAMIC_DRAW));
    //if (count > indexCount)
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
    //else
        //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count*sizeof(unsigned int), data);
    //GLCall();
    indexCount = count;
    //GLCall(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count*sizeof(unsigned int), data));
}
void IndexBuffer::bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID));
}
void IndexBuffer::unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}