#include <Galaxy/ECS/ecs.hpp>

auto ECSManager::main = std::make_unique<ECSManager>();
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
    EntityData &self = manager.entities[id];

    for (auto &system : manager.systems)
    {
        ComponentMask mask = system->compMask;
        if ((mask & self.components) == mask)
        {
            system->_entities.insert(id);
            self.systemsContaining.push_back(system.get());
        }
    }

    return Entity{.id=id};
}
void Entity::destroy(ECSManager &manager)
{
    manager.availableIDs.push(id);
    EntityData &self = manager.entities[id];
    self.components = 0;

    for (auto &system : self.systemsContaining)
        system->_entities.erase(id);
}