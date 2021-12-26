#pragma once
#include <vector>

//class UIObject
//{
//private:
//    std::unique_ptr<std::vector<UIObject*>> uiObjects;
//    friend class Renderer;
//protected:
//    unsigned int rendererID;
//    int renderOrder = 0;
//public:

//};
class UIImage;
class UIText;
struct UIObject
{
    static std::unique_ptr<std::vector<UIObject*>> uiObjects;
    void *data;
    //int typeID; // possibly use
    enum Type { UIIMAGE, UITEXT } type;

    unsigned int &renderer_id();
    int &render_order();

    static int add_image(UIImage *img);
    static int add_text(UIText *text);
    static void remove(int id);
    
    static bool sortObjects;

    UIObject(void *data, Type type);
    UIObject(const UIObject&) = delete;
    void operator=(const UIObject&) = delete;
};