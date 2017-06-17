#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <generic_ptrs.hpp>
#include <non_copyable_movable.hpp>
#include "ResourceLocator.hpp"

class ResourceManager : util::non_copyable_movable
{
    std::unordered_map<std::string,util::generic_shared_ptr> cache;
    std::unique_ptr<ResourceLocator> locator;

public:
    ResourceManager();
    ~ResourceManager();

    util::generic_shared_ptr load(std::string id);

    template <typename T>
    std::shared_ptr<T> load(std::string id) { return load(id).as<T>(); }

    void collectUnusedResources();

    ResourceLocator* getResourceLocator() { return locator.get(); }
    void setResourceLocator(ResourceLocator* loc) { locator.reset(loc); }
};
