#pragma once
class Scene;


// For quick debugging windows (possibly use UIGroup instead)
class UIWindow
{
private:
    Scene *scene;
public:
    UIWindow();
    ~UIWindow();

    UIWindow(UIWindow const&) = delete;
    UIWindow &operator=(UIWindow const&) = delete;
    //UIWindow(UIWindow&&) = delete;
    //UIWindow &operator=(UIWindow&&) = delete;
};