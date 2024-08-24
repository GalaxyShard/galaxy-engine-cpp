#include <cassert>
#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/text.hpp>

#include "uiobject.hpp"
auto UIObject::uiObjects = new std::vector<UIObject>();
bool UIObject::sortObjects = 0;

UIObject::UIObject(void *data, Type type) : data(data), type(type) { }
int UIObject::add_image(UIImage *img)
{
    uiObjects->emplace_back(img, UIIMAGE);
    sortObjects = 1;
    return uiObjects->size()-1;
}
int UIObject::add_text(UIText *text)
{
    uiObjects->emplace_back(text, UITEXT);
    sortObjects = 1;
    return uiObjects->size()-1;
}
void UIObject::remove(unsigned int id)
{
    uiObjects->erase(uiObjects->begin() + id);
    for (unsigned int i = id; i < uiObjects->size(); ++i)
    {
        (*uiObjects)[i].renderer_id() = i;
    }
}
unsigned int &UIObject::renderer_id()
{
    if (type == UIIMAGE) return ((UIImage*)data)->rendererID;
    else if (type == UITEXT) return ((UIText*)data)->rendererID;
    assert(false && "Type not implemented");
}
int &UIObject::render_order()
{
    if (type == UIIMAGE) return ((UIImage*)data)->renderOrder;
    else if (type == UITEXT) return ((UIText*)data)->renderOrder;
    assert(false && "Type not implemented");
}