#pragma once
#include <vector>

class UIImage;
class UIText;
struct UIObject
{
    //static std::vector<UIObject*> *uiObjects;
    static std::vector<UIObject> *uiObjects;
    void *data;
    enum Type : unsigned char { UIIMAGE, UITEXT } type;
    
    unsigned int &renderer_id();
    int &render_order();

    static int add_image(UIImage *img);
    static int add_text(UIText *text);
    static void remove(unsigned int id);
    
    static bool sortObjects;

    UIObject(void *data, Type type);
    //UIObject(const UIObject&) = delete;
    //void operator=(const UIObject&) = delete;
};