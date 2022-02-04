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
public:
    static std::unique_ptr<UIGroup> aspectRatio, safeArea;
    UIGroup *parent = 0;

    Vector2 pos, scale = Vector2(1,1), anchor = Vector2();

    Vector2 world_pos();
    Vector2 world_scale();

    UIGroup();
    ~UIGroup();
};