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
    //std::vector<Entity> _entities;
    std::set<EntityID> _entities;
    ComponentMask compMask;
    friend class ECSManager;
    friend struct Entity;
public:
    virtual ~SystemBase() = default;
};
//template<typename... Args>
//class System;

struct EntityData
{
    std::unordered_map<unsigned int, int> indices;
    std::vector<unsigned char> raw;
    //std::vector<int> rawIndices;
    //std::vector<std::vector<unsigned char>> raw;
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
    //std::vector<Entity>& entities() { return _entities; }
    //std::set<EntityID>& entities() { return _entities; }
    static ComponentMask mask(ECSManager &manager);
    //void for_each(Args&... args);
    void run(void (T::*)(Args&...), ECSManager &manager);
};
#include "ecs.inl"