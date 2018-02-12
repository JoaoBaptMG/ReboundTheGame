#include "FilesystemResourceLocator.hpp"
#include <fstream>

inline static std::string getResourceDirectory()
{
    return getExecutableDirectory() + "/Resources";
}

FilesystemResourceLocator::FilesystemResourceLocator() : basename(getResourceDirectory())
{
}

std::unique_ptr<sf::InputStream> FilesystemResourceLocator::getResource(std::string name)
{
    std::string fullname = basename + '/' + name;

    std::unique_ptr<sf::FileInputStream> ptr{new sf::FileInputStream()};
    if (!ptr->open(fullname)) throw FileNotFound(name);

    return std::unique_ptr<sf::InputStream>{ptr.release()};
}
