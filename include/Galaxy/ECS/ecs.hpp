#pragma once
#include <unordered_map>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>

typedef unsigned int ComponentMask;
typedef unsigned int SystemMask;
typedef unsigned int EntityID;
struct Entity;
class SystemBase
{
protected:
    std::set<EntityID> _entities;
    ComponentMask compMask;
    friend class ECSManager;
    friend struct Entity;
public:
    //virtual ~SystemBase() = 0;
};
struct EntityData
{
    std::unordered_map<unsigned int, int> indices;
    std::vector<unsigned char> raw;
    EntityID id;
    ComponentMask components;
    std::vector<SystemBase*> systemsContaining;
};
class ECSManager
{
private:
    std::vector<EntityData> entities;
    std::queue<EntityID> availableIDs;
    std::vector<std::unique_ptr<SystemBase>> systems;
    
    unsigned int nextSysID = 0;
    unsigned int nextCompID = 0;

    friend struct Entity;
public:
    static std::unique_ptr<ECSManager> main;

    template<typename T>
    T* reg_sys();

    template<typename T>
    T* get_sys();

    template<typename T>
    unsigned int comp_id();
    template<typename T>
    unsigned int sys_id();

    template<typename T>
    ComponentMask comp_mask();
    template<typename T>
    SystemMask sys_mask();
};
struct Entity
{
    EntityID id;
    
    template<typename T>
    void add_comp(T data, ECSManager &manager);
    
    template<typename T>
    void remove_comp(ECSManager &manager);

    static Entity create(ECSManager &manager);
    void destroy(ECSManager &manager);

    template<typename T>
    T& get_comp(ECSManager &manager);
};

template<typename T, typename... Args>
class System : public SystemBase
{
public:
    static ComponentMask mask(ECSManager &manager);
    void run(void (T::*)(Args&...), ECSManager &manager);
};
#include "ecs.inl"