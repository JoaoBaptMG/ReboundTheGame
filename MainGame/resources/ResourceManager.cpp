#include "ResourceManager.hpp"
#include "ResourceLoader.hpp"

using namespace util;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

generic_shared_ptr ResourceManager::load(std::string id)
{
    auto it = cache.find(id);

    if (it == cache.end())
    {
        auto ptr = ResourceLoader::loadFromStream(locator->getResource(id));
        if (!ptr) throw ResourceNotIdentified(id);
        std::tie(it, std::ignore) = cache.emplace(id, ptr);
    }

    return it->second;
}

void ResourceManager::collectUnusedResources()
{
    for (auto it = cache.begin(); it != cache.end();)
    {
        if (it->second.use_count() == 1)
            it = cache.erase(it);
        else ++it;
    }
}
