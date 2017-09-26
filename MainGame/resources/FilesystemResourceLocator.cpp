#include "FilesystemResourceLocator.hpp"
#include <fstream>

std::unique_ptr<sf::InputStream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<sf::FileInputStream> ptr{new sf::FileInputStream()};
    if (!ptr->open(fullname))
        throw FileNotFound(name);

    return std::unique_ptr<sf::InputStream>{ptr.release()};
}
