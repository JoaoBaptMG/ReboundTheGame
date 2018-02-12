#pragma once

#include <type_id.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <exception>

class LevelPersistentData final
{
    struct DataDetail
    {
        using Ptr = std::unique_ptr<void, void(*)(void*)>;
        Ptr ptr;
        util::type_id_t type;
    };
    
    std::unordered_map<std::string,DataDetail> data;
    
    void setDataImpl(std::string key, void* ptr, void(*deleter)(void*), util::type_id_t type);
    void* getDataImpl(std::string key, util::type_id_t type);
    bool existsDataOfTypeImpl(std::string key, util::type_id_t type);
    
public:
    LevelPersistentData();
    ~LevelPersistentData() {}
    
    template <typename T>
    void setData(std::string key, const T& data)
    {
        setDataImpl(key, new T(data), [](void* p) { delete static_cast<T*>(p); }, util::type_id<T>());
    }
    
    template <typename T>
    T getData(std::string key)
    {
        return *static_cast<T*>(getDataImpl(key, util::type_id<T>()));
    }
    
    template <typename T>
    bool existsDataOfType(std::string key)
    {
        return existsDataOfTypeImpl(key, util::type_id<T>());
    }
    
    void clear();
};

struct InvalidLevelPersistentDataAccessError final : public std::runtime_error
{
    InvalidLevelPersistentDataAccessError(std::string key, std::string msg) :
        std::runtime_error("Exception while accessing level persistent data with key " + key + ": " + msg) {}
};
