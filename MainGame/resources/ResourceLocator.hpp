#pragma once

#include <SFML/System.hpp>

class ResourceLocator
{
    ResourceLocator(const ResourceLocator&) = delete;
    void operator=(const ResourceLocator&) = delete;

public:
    ResourceLocator() {}
    virtual ~ResourceLocator() {}

    virtual std::unique_ptr<sf::InputStream> getResource(std::string name) = 0;
};

