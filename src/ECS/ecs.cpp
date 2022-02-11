#include <Galaxy/ECS/ecs.hpp>

Entity Entity::create(ECSManager &manager)
{
    if (manager.availableIDs.size())
    {
        EntityID id = manager.availableIDs.front();
        manager.availableIDs.pop();
        return Entity{.id=id};
    }
    EntityID id = (EntityID)manager.entities.size();
    manager.entities.push_back(EntityData{ .id=id, .components=0 });
    return Entity{.id=id};
}
void Entity::destroy(ECSManager &manager)
{
    manager.availableIDs.push(id);
    EntityData &self = manager.entities[id];
    self.components = 0;

}