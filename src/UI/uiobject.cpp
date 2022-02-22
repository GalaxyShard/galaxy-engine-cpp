#include <Galaxy/UI/image.hpp>
#include <Galaxy/UI/text.hpp>

#include "uiobject.hpp"
auto UIObject::uiObjects = std::make_unique<std::vector<UIObject*>>();
bool UIObject::sortObjects = 0;

UIObject::UIObject(void *data, Type type) : data(data), type(type) { }
int UIObject::add_image(UIImage *img)
{
    UIObject *obj = new UIObject(img, UIIMAGE);
    uiObjects->push_back(obj);
    sortObjects = 1;
    return uiObjects->size()-1;
}
int UIObject::add_text(UIText *text)
{
    UIObject *obj = new UIObject(text, UITEXT);
    uiObjects->push_back(obj);
    sortObjects = 1;
    return uiObjects->size()-1;
}
void UIObject::remove(int id)
{
    delete (*uiObjects)[id];
    uiObjects->erase(uiObjects->begin() + id);
    for (int i = id; i < uiObjects->size(); ++i)
    {
        (*uiObjects)[i]->renderer_id() = i;
    }
}
unsigned int &UIObject::renderer_id()
{
    if (type == UIIMAGE) return ((UIImage*)data)->rendererID;
    else if (type == UITEXT) return ((UIText*)data)->rendererID;
    assert(false);
}
int &UIObject::render_order()
{
    if (type == UIIMAGE) return ((UIImage*)data)->renderOrder;
    else if (type == UITEXT) return ((UIText*)data)->renderOrder;
    assert(false);
}