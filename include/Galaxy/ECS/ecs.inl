
template <typename... Args>
ComponentMask comp_bit(ECSManager &manager)
{
    return (... | manager.comp_flag<Args>());
}
template<typename T, typename... Args>
void ECSManager::reg_sys()
{
    ComponentMask bits = comp_bit<Args...>(*this);
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
ComponentMask ECSManager::comp_flag()
{
    static ComponentMask flag = (ComponentMask)1 << comp_id<T>();
    return flag;
}
template<typename T>
SystemMask ECSManager::sys_flag()
{
    static SystemMask flag = (SystemMask)1 << sys_id<T>();
    return flag;
}

template<typename T>
void Entity::add_comp(T data, ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    int index = self.raw.size();
    self.raw.push_back(std::vector<unsigned char>());
    self.raw[index].resize(sizeof(T));
    memcpy(self.raw[index].data(), &data, sizeof(T));
    self.indices[manager.comp_id<T>()] = index;
    self.components |= manager.comp_flag<T>();
}
template<typename T>
void Entity::remove_comp(ECSManager &manager)
{
    EntityData &self = manager.entities[id];
    self.components &= ~manager.comp_flag<T>();
    int index = self.indices[manager.comp_id<T>()];

    self.raw.erase(self.raw.begin()+index);
    for (auto &[key, value] : self.indices)
    {
        if (value > index)
            value -= 1;
    }
}