#pragma once
#include <memory>
#include <Galaxy/Math/vector.hpp>
class UIImage;
class UIText;
class Scene;

class UIGroup
{
private:
    Scene *scene;
    friend class Scene;
public:
    static UIGroup *aspectRatio, *safeArea;
    UIGroup *parent = 0;

    Vector2 pos, scale = Vector2(1,1), anchor = Vector2();
private:
    int sceneID;
public:

    Vector2 world_pos();
    Vector2 world_scale();

    UIGroup();
    ~UIGroup();
};