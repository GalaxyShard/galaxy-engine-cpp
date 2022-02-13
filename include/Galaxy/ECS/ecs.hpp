#pragma once
#include <unordered_map>
#include <vector>
#include <queue>

typedef unsigned int ComponentMask;
typedef unsigned int SystemMask;
typedef unsigned int EntityID;
struct Entity;
struct EntityData
{
    std::unordered_map<unsigned int, int> indices;
    std::vector<std::vector<unsigned char>> raw;
    EntityID id;
    ComponentMask components;
};
class ECSManager
{
private:
    std::vector<EntityData> entities;
    std::queue<EntityID> availableIDs;
    
    unsigned int nextSysID = 0;
    unsigned int nextCompID = 0;

    friend struct Entity;
public:

    //template<typename T, typename... Args>
    //void reg_sys();
    template<typename T>
    void reg_sys();

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
//class BaseSystem { };
template<typename... Args>
class System
{
    friend class ECSManager;
public:
    static ComponentMask mask(ECSManager &manager);
    void run();
};
#include "ecs.inl"