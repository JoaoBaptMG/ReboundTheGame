#pragma once

#include <string>
#include "../utility/generic_ptrs.hpp"
#include <SFML/System.hpp>

namespace ResourceLoader
{
    util::generic_shared_ptr loadFromStream(std::unique_ptr<sf::InputStream> stream);
}

class ResourceNotIdentified : std::runtime_error
{
public:
    inline ResourceNotIdentified(std::string name)
    : std::runtime_error("Resource loaded from index " + name + "could not be identified!") {}
};

