#pragma once
#include <memory>
#include <Galaxy/Math/vector.hpp>
class UIImage;
class UIText;
class Scene;

class UIGroup
{
private:
    Scene *scene=0;
    UIGroup *m_parent = 0;

    friend class Scene;
    UIGroup() = default;
public:
    static UIGroup *aspectRatio, *safeArea;

    Vector2 pos, scale = Vector2(1,1), anchor;
private:
    Vector2 m_worldPos;
    Vector2 m_worldScale = Vector2(1,1);
    int sceneID;
    Vector2 calc_pos();
    Vector2 calc_scale();
    Vector2 calc_aspect(Vector2 worldScale);
public:
    void parent(UIGroup *g);
    Vector2 world_pos();
    Vector2 world_scale();
    Vector2 world_aspect();

    // Run every time parent, pos, scale, or anchor changes
    void cache();

    static UIGroup *create();
    static void destroy(UIGroup *group);
    ~UIGroup();
};