#include "GameObjectFactory.hpp"

#include <new>
#include <unordered_map>
#include "data/RoomData.hpp"

#include <iostream>

using FactoryParamMap = std::unordered_map<std::string, FactoryParams>;
static size_t SchwartzCounter = 0;
static typename std::aligned_storage_t<sizeof(FactoryParamMap), alignof(FactoryParamMap)> factoryParamsBuf;
FactoryParamMap& factoryParams = reinterpret_cast<FactoryParamMap&>(factoryParamsBuf);

BaseRegisterGameObject::BaseRegisterGameObject(const std::string& name, FactoryParams params)
{
    if (SchwartzCounter++ == 0) new (&factoryParams) FactoryParamMap();
    factoryParams.emplace(name, params);

    std::cout << "Registering " << name << " into factory..." << std::endl;
}

BaseRegisterGameObject::~BaseRegisterGameObject()
{
    if (--SchwartzCounter == 0) (&factoryParams)->~FactoryParamMap();
}

util::generic_shared_ptr readParametersFromStream(sf::InputStream& stream, std::string klass)
{
    if (SchwartzCounter == 0) return util::generic_shared_ptr{};

    auto it = factoryParams.find(klass);

    if (it == factoryParams.end())
        return util::generic_shared_ptr{};

    return it->second.reader(stream);
}

std::unique_ptr<GameObject> createObjectFromDescriptor(GameScene& gameScene, const GameObjectDescriptor& descriptor)
{
    if (SchwartzCounter == 0) return std::unique_ptr<GameObject>{};

    auto it = factoryParams.find(descriptor.klass);

    if (it == factoryParams.end())
        return std::unique_ptr<GameObject>{};

    auto obj = it->second.factory(gameScene, descriptor.name, descriptor.parameters);
    return obj;
}
