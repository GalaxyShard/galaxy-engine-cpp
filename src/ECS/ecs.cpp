#include <Galaxy/ECS/ecs.hpp>

auto ECSManager::main = std::make_unique<ECSManager>();
Entity Entity::create(ECSManager &manager)
{
    if (manager.availableIDs.size())
    {
        EntityID id = manager.availableIDs.front();
        manager.availableIDs.pop();
        return Entity{id};
    }
    EntityID id = (EntityID)manager.entities.size();
    EntityData data;
    data.id = (EntityID)manager.entities.size();
    data.components = 0;
    manager.entities.push_back(data);
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
    return Entity{id};
}
void Entity::destroy(ECSManager &manager)
{
    manager.availableIDs.push(id);
    EntityData &self = manager.entities[id];
    self.components = 0;

    for (auto &system : self.systemsContaining)
        system->_entities.erase(id);
}