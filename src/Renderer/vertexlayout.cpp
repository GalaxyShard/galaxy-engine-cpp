#include <Renderer/buffer.hpp>
#include <unordered_map>

std::unordered_map<AttributeType, int> typeToBytes =
{
    {AttributeType::BYTE,1},
    {AttributeType::UBYTE,1},
    {AttributeType::SHORT,2},
    {AttributeType::USHORT,2},
    {AttributeType::INT,4},
    {AttributeType::UINT,4},
    {AttributeType::FLOAT,4},
    {AttributeType::DOUBLE,8},
    {AttributeType::HALF,2}
};
void VertexLayout::add_attribute(VertexAttribute a)
{
    assert(a.type != AttributeType::NONE);
    attributes.push_back(a);
    stride += a.count * typeToBytes[a.type];
}