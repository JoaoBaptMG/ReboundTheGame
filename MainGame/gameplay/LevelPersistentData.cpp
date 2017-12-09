#include "LevelPersistentData.hpp"

LevelPersistentData::LevelPersistentData()
{
}

void LevelPersistentData::setDataImpl(std::string key, void* ptr, void(*deleter)(void*), util::type_id_t type)
{
    auto it = data.find(key);
    
    if (it == data.end())
        data.emplace(key, DataDetail{ DataDetail::Ptr{ptr, deleter}, type });
    else if (it->second.type == type)
        it->second.ptr.reset(ptr);
    else it->second.ptr = DataDetail::Ptr{ptr, deleter};
}

void* LevelPersistentData::getDataImpl(std::string key, util::type_id_t type)
{
    auto it = data.find(key);
    
    if (it == data.end())
        throw InvalidLevelPersistentDataAccessError(key, "data does not exist!");
    else if (it->second.type != type)
        throw InvalidLevelPersistentDataAccessError(key, "data has invalid type!");
    else return it->second.ptr.get();
}

bool LevelPersistentData::existsDataOfTypeImpl(std::string key, util::type_id_t type)
{
    auto it = data.find(key);
    return it != data.end() && it->second.type == type;
}

void LevelPersistentData::clear()
{
    data.clear();
}
    
