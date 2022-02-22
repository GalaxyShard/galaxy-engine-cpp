
template <typename... Args>
ComponentMask comp_mask(ECSManager &manager)
{
    return (... | manager.comp_mask<Args>());
}
template<typename T>
T* ECSManager::reg_sys()
{
    static_assert(std::is_base_of<SystemBase, T>::value, "Class must inherit System");

    ComponentMask mask = T::mask(*this);
    systems.push_back(std::unique_ptr<SystemBase>(new T()));
    T *system = (T*)systems[systems.size()-1].get();
    system->compMask = mask;
    for (auto &entity : entities)
    {
        if ((mask & entity.components) == mask)
        {
            system->_entities.insert(entity.id);
            entity.systemsContaining.push_back(system);
        }
    }
    return system;
}
template<typename T>
T* ECSManager::get_sys()
{
    static_assert(std::is_base_of<SystemBase, T>::value, "Class must inherit System");
    for (auto &system : systems)
    {
        if (T *sys = dynamic_cast<T*>(system.get()))
        {
            return sys;
        }
    }
    return 0;
}
template<typename T>
unsigned int ECSManager::comp_id()
{
    static int id = nextCompID++;
    if (id >= sizeof(ComponentMask)*8)
        throw("Too many components");
    return id;
}
template<typename T>
unsigned int ECSManager::sys_id()
{
    static unsigned int id = nextSysID++;
    if (id >= sizeof(SystemMask)*8)
        throw("Too many systems");
    return id;
    
}
template<typename T>
ComponentMask ECSManager::comp_mask()
{
    static ComponentMask mask = (ComponentMask)1 << comp_id<T>();
    return mask;
}
template<typename T>
SystemMask ECSManager::sys_mask()
{
    static SystemMask mask = (SystemMask)1 << sys_id<T>();
    return mask;
}

template<typename T, typename... Args>
ComponentMask System<T, Args...>::mask(ECSManager &manager)
{
    return ::comp_mask<Args...>(manager);
}
template<typename T, typename... Args>
void System<T, Args...>::run(void (T::*func)(Args&...), ECSManager &manager)
{
    for (auto &id : _entities)
    {
        Entity e = Entity{id};
        (((T*)this)->*func)(e.get_comp<Args>(manager)...);
    }
}

template<typename T>
void Entity::add_comp(T data, ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    if (self.components & manager.comp_mask<T>())
        throw("Already added component to entity");

    int index = self.raw.size();
    self.raw.resize(self.raw.size()+sizeof(T));
    memcpy(&self.raw[index], &data, sizeof(T));
    self.indices[manager.comp_id<T>()] = index;
    self.components |= manager.comp_mask<T>();

    for (auto &system : manager.systems)
    {
        ComponentMask mask = system->compMask;
        if ((mask & self.components) == mask)
        {
            if (!system->_entities.count(id))
            {
                system->_entities.insert(id);
                self.systemsContaining.push_back(system.get());
            }
        }
    }
}
template<typename T>
void Entity::remove_comp(ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    if (!(self.components & manager.comp_mask<T>()))
        throw("Removed component does not exist on entity");
    self.components &= ~manager.comp_mask<T>();
    int index = self.indices[manager.comp_id<T>()];

    auto startIter = self.raw.begin()+index;
    self.raw.erase(startIter, startIter+sizeof(T));
    self.indices.erase(manager.comp_id<T>());

    for (auto &[key, value] : self.indices)
    {
        if (value > index)
            value -= sizeof(T);
    }
    for (auto &system : self.systemsContaining)
    {
        if ((system->compMask & self.components) != system->compMask)
            system->_entities.erase(id);
    }
}
template<typename T>
T& Entity::get_comp(ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    //if (!(self.components & manager.comp_mask<T>()))
    //    throw("Component does not exist on entity");
    //T &comp = *(T*)self.raw[self.indices[manager.comp_id<T>()]].data();
    T &comp = *(T*)&self.raw[self.indices[manager.comp_id<T>()]];
    return comp;
}