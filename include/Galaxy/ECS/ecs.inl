
template <typename... Args>
ComponentMask comp_mask(ECSManager &manager)
{
    return (... | manager.comp_mask<Args>());
}
//template<typename T, typename... Args>
//void ECSManager::reg_sys()
//{
//    ComponentMask bits = comp_mask<Args...>(*this);
//}
template<typename T>
void ECSManager::reg_sys()
{
    //static_assert(std::is_base_of<System<>, T>::value, "Class must inherit System");
    ComponentMask mask = T::mask(*this);

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

template<typename... Args>
ComponentMask System<Args...>::mask(ECSManager &manager)
{
    return ::comp_mask<Args...>(manager);
}
template<typename... Args>
void System<Args...>::run()
{

}

template<typename T>
void Entity::add_comp(T data, ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    if (self.components & manager.comp_mask<T>())
        throw("Already added component to entity");

    int index = self.raw.size();
    self.raw.push_back(std::vector<unsigned char>(sizeof(T)));
    self.raw[index].resize(sizeof(T));
    memcpy(self.raw[index].data(), &data, sizeof(T));
    self.indices[manager.comp_id<T>()] = index;
    self.components |= manager.comp_mask<T>();
}
template<typename T>
void Entity::remove_comp(ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    if (!(self.components & manager.comp_mask<T>()))
        throw("Removed component does not exist on entity");
    self.components &= ~manager.comp_mask<T>();
    int index = self.indices[manager.comp_id<T>()];

    self.raw.erase(self.raw.begin()+index);
    for (auto &[key, value] : self.indices)
    {
        if (value > index)
            value -= 1;
    }
}