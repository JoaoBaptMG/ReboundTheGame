#pragma once

#include <string>
#include <SFML/System.hpp>

#include <generic_ptrs.hpp>

namespace ResourceLoader
{
    util::generic_shared_ptr loadFromStream(std::unique_ptr<sf::InputStream> stream, std::string type);
}

class ResourceLoadingError : public std::runtime_error
{
public:
    inline ResourceLoadingError(std::string name)
    : std::runtime_error("Error loading resource from index " + name + "!") {}
};

